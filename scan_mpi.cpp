#include <iostream>
#include <vector>
#include <fstream>
#include <limits>
#include <mpi.h>

using namespace std;

void validatePrefixSum(const vector<int> &temp, const vector<int> &final)
{
    int curr = 0;

    for (const auto &t : temp)
    {
        curr += t;

        if (final[&t - &temp[0]] != curr)
        {
            printf("INVALID PREFIX SUM\n");
            exit(1);
        }
    }

    printf("VALID PREFIX SUM\n");
}

void checkIfPowerOfTwo(int n)
{
    // Check if the size of the input array is a power of two
    if (n == 0)
    {
        cout << "Not a power of two" << endl;
        exit(1);
    }

    while (n != 1)
    {
        if (n % 2 != 0)
        {
            cout << "Not a power of two" << endl;
            exit(1);
        }
        n = n / 2;
    }
}

void blelloch_scan(vector<int> &nums)
{
    int n = nums.size();

    // Up-sweep (reduce) phase
    int last = 0;
    for (int d = 1; d < n; d *= 2)
    {
        for (int i = 0; i < n; i += 2 * d)
        {
            nums[i + 2 * d - 1] += nums[i + d - 1];
        }
    }
    last = nums[n - 1];

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

    // Increase the size of the array by 1 and set the last element to the last value of the up-sweep phase
    nums.resize(n + 1);
    nums[n] = last;

    // Remove the first element of the array
    nums.erase(nums.begin());
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Read input from CSV file on rank 0
    vector<int> nums;
    int num;
    if (rank == 0)
    {
        ifstream input_file("input.bin", ios::binary);

        while (input_file.read(reinterpret_cast<char *>(&num), sizeof(int)))
        {
            nums.push_back(num);
        }
    }

    // Check if the size of the input array is a power of two
    if (rank == 0)
    {
        checkIfPowerOfTwo(nums.size());
    }

    // Broadcast the size of the input array to all processes
    int n = nums.size();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate the size of each process's subset of the input data
    int chunk_size = n / size;
    if (n % size != 0)
    {
        chunk_size++;
    }

    // Allocate memory for each process's subset of the input data
    vector<int> chunk(chunk_size);

    // Scatter the input data across processes
    MPI_Scatter(nums.data(), chunk_size, MPI_INT, chunk.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform the Blelloch scan algorithm on each process's subset of the input data
    blelloch_scan(chunk);

    // Gather the results from all processes onto rank 0
    vector<int> result;
    if (rank == 0)
    {
        result.resize(n);
    }
    MPI_Gather(chunk.data(), chunk_size, MPI_INT, result.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Combine the results from all processes
    if (rank == 0)
    {
        vector<int> temp;
        temp.push_back(0);
        for (int i = 0; i < size; i++)
        {
            temp.push_back(result[i * chunk_size + chunk_size - 1]);
        }
        blelloch_scan(temp);
        for (int i = 0; i < size; i++)
        {
            result[i * chunk_size + chunk_size - 1] = temp[i + 1];
        }
    }

    // Broadcast the combined results to all processes
    MPI_Bcast(result.data(), n, MPI_INT, 0, MPI_COMM_WORLD);

    // Print the results
    if (rank == 0)
    {
        validatePrefixSum(nums, result);
    }

    MPI_Finalize();

    return 0;
}