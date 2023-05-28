#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <limits>
#include <mpi.h>

using namespace std;

void validatePrefixSum(const vector<int> &temp, const vector<int> &final)
{
    int curr = 0;

    for (size_t i = 0; i < temp.size(); i++)
    {
        curr += temp[i];

        if (final[i] != curr)
        {
            printf("INVALID PREFIX SUM\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    printf("VALID PREFIX SUM\n");
}

void blelloch_scan(vector<int> &nums)
{

    int n = nums.size();

    // Up-sweep (reduce) phase
    for (int d = 1; d < n; d *= 2)
    {
        for (int i = 0; i < n; i += 2 * d)
        {
            nums[i + 2 * d - 1] += nums[i + d - 1];
        }
    }

    // Set root to 0
    nums[n - 1] = 0;

    // Down-sweep (scan) phase
    for (int d = n / 2; d > 0; d /= 2)
    {
        for (int i = 0; i < n; i += 2 * d)
        {
            int t = nums[i + d - 1];
            nums[i + d - 1] = nums[i + 2 * d - 1];
            nums[i + 2 * d - 1] += t;
        }
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int numProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    int processRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    std::vector<int> inputData;
    int chunkSize;

    if (processRank == 0)
    {
        int number;
        std::ifstream inputFile("../input.bin", std::ios::binary);

        while (inputFile.read(reinterpret_cast<char *>(&number), sizeof(int)))
        {
            inputData.push_back(number);
        }

        int size = inputData.size();
        chunkSize = size / numProcesses;
    }

    double startTime = MPI_Wtime();

    MPI_Bcast(&chunkSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> localInput(chunkSize);

    MPI_Scatter(inputData.data(), chunkSize, MPI_INT, localInput.data(), chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    int localSum;

    for (size_t i = 1; i < localInput.size(); i++)
    {
        localInput[i] += localInput[i - 1];
        if (i == localInput.size() - 1)
        {
            localSum = localInput[i];
        }
    }

    std::vector<int> globalSumArray(numProcesses);
    MPI_Gather(&localSum, 1, MPI_INT, globalSumArray.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (processRank == 0)
    {
        blelloch_scan(globalSumArray);
    }

    int sumToAdd;
    MPI_Scatter(globalSumArray.data(), 1, MPI_INT, &sumToAdd, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (size_t i = 0; i < localInput.size(); i++)
    {
        localInput[i] += sumToAdd;
    }

    std::vector<int> finalData(numProcesses * chunkSize);
    MPI_Gather(localInput.data(), chunkSize, MPI_INT, finalData.data(), chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    double endTime = MPI_Wtime();

    if (processRank == 0)
    {
        printf("Time taken: %f seconds\n", (endTime - startTime) * 1000);

        validatePrefixSum(inputData, finalData);

        std::ofstream outputFile("output.csv", std::ios::app);
        outputFile << (endTime - startTime) * 1000 << std::endl;
    }

    MPI_Finalize();

    return 0;
}