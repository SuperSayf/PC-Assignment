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
    for (int d = 1; d < n; d *= 2) // Doubles the distance between elements in each iteration.
    {
        for (int i = 0; i < n; i += 2 * d) // Iterates over the array in strides of 2*d.
        {
            nums[i + 2 * d - 1] += nums[i + d - 1]; // Adds the value at index i+d-1 to the value at index i+2*d-1.
        }
    }

    // Set root to 0
    nums[n - 1] = 0; // Sets the last element of the array to 0.

    // Down-sweep (scan) phase
    for (int d = n / 2; d > 0; d /= 2) // Halves the distance between elements in each iteration.
    {
        for (int i = 0; i < n; i += 2 * d) // Iterates over the array in strides of 2*d.
        {
            int t = nums[i + d - 1];               // Stores the value at index i+d-1 in a temporary variable.
            nums[i + d - 1] = nums[i + 2 * d - 1]; // Sets the value at index i+2*d-1 to the value at index i+d-1.
            nums[i + 2 * d - 1] += t;              // Adds the temporary value to the value at index i+2*d-1.
        }
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv); // Initializes the MPI environment.

    int numProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses); // Gets the total number of processes.

    int processRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // Gets the rank (identifier) of the current process.

    std::vector<int> inputData; // Creates a vector to store input data.
    int chunkSize;              // Represents the size of each chunk of data.

    if (processRank == 0) // Only the process with rank 0 executes this block.
    {
        int number;
        std::ifstream inputFile("../input.bin", std::ios::binary); // Opens a binary file for reading.

        while (inputFile.read(reinterpret_cast<char *>(&number), sizeof(int))) // Reads integers from the file.
        {
            inputData.push_back(number); // Adds the read number to the input data vector.
        }

        int size = inputData.size();
        chunkSize = size / numProcesses; // Calculates the size of each chunk of data.
    }

    double startTime = MPI_Wtime(); // Gets the current time.

    MPI_Bcast(&chunkSize, 1, MPI_INT, 0, MPI_COMM_WORLD); // Broadcasts the chunk size to all processes.

    std::vector<int> localInput(chunkSize); // Creates a vector to store the local input data for each process.

    MPI_Scatter(inputData.data(), chunkSize, MPI_INT, localInput.data(), chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
    // Scatters the input data to all processes, each receiving a chunk of data.

    int localSum;

    for (size_t i = 1; i < localInput.size(); i++) // Loops through the local input data.
    {
        localInput[i] += localInput[i - 1]; // Adds the previous element to the current element.
        if (i == localInput.size() - 1)     // Checks if it's the last element.
        {
            localSum = localInput[i]; // Stores the sum of the local input elements.
        }
    }

    std::vector<int> globalSumArray(numProcesses); // Creates a vector to store global sums.

    MPI_Gather(&localSum, 1, MPI_INT, globalSumArray.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Gathers the local sums from all processes to the global sum array on process 0.

    if (processRank == 0) // Only the process with rank 0 executes this block.
    {
        blelloch_scan(globalSumArray); // Performs the Blelloch scan on the global sum array.
    }

    int sumToAdd;
    MPI_Scatter(globalSumArray.data(), 1, MPI_INT, &sumToAdd, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Scatters a single global sum to each process.

    for (size_t i = 0; i < localInput.size(); i++) // Loops through the local input data.
    {
        localInput[i] += sumToAdd; // Adds the sum to each element.
    }

    std::vector<int> finalData(numProcesses * chunkSize); // Creates a vector to store the final data.

    MPI_Gather(localInput.data(), chunkSize, MPI_INT, finalData.data(), chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
    // Gathers the modified local input data from all processes to the final data array on process 0.

    double endTime = MPI_Wtime(); // Gets the current time.

    if (processRank == 0) // Only the process with rank 0 executes this block.
    {
        printf("Time taken: %f seconds\n", (endTime - startTime) * 1000); // Prints the time taken.

        validatePrefixSum(inputData, finalData); // Validates the prefix sum.

        std::ofstream outputFile("output.csv", std::ios::app);   // Opens a file for writing.
        outputFile << (endTime - startTime) * 1000 << std::endl; // Writes the time taken to the file.
    }

    MPI_Finalize(); // Terminates the MPI environment.

    return 0; // Exits the program.
}
