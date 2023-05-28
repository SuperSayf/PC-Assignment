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
            MPI_Abort(MPI_COMM_WORLD, 1); // Abort MPI processes if prefix sum is invalid
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
            int left_child = i + d - 1;
            int right_child = i + 2 * d - 1;
            nums[right_child] += nums[left_child];
        }
    }

    // Set the last element to 0
    nums[n - 1] = 0;

    // Down-sweep (scan) phase
    for (int d = n / 2; d > 0; d /= 2)
    {
        for (int i = 0; i < n; i += 2 * d)
        {
            int left_child = i + d - 1;
            int right_child = i + 2 * d - 1;
            nums[right_child] += nums[left_child];
            nums[left_child] = nums[right_child] - nums[left_child];
        }
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // // Read input from bin file on process 0
    // vector<int> input;
    // if (world_rank == 0) {
    //     int num;
    //     ifstream input_file("input.bin", ios::binary);

    //     while (input_file.read(reinterpret_cast<char *>(&num), sizeof(int))) {
    //         input.push_back(num);
    //     }
    // }
    // Set input with some values

    // Decalred on all processes
    vector<int> input;
    int chunk_size;

    // If the rank is 0, define the input vector
    if (world_rank == 0)
    {
        // Read in the input vector from the input.bin file
        int num;
        ifstream input_file("../input.bin", ios::binary);

        while (input_file.read(reinterpret_cast<char *>(&num), sizeof(int)))
        {
            input.push_back(num);
        }

        // Calculate how much each process must get from the input vector
        int size = input.size();
        chunk_size = size / world_size;
    }
    // Start the time using omp_get_wtime()
    double start = MPI_Wtime();

    // Broadcast the chunk size to all processes
    MPI_Bcast(&chunk_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Declare the local input vector on all processes
    vector<int> local_input(chunk_size);

    // Scatter the input vector to all processes
    MPI_Scatter(input.data(), chunk_size, MPI_INT, local_input.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    int grandSum;

    // Now on each process we need to compute the scan of the local input vector
    for (size_t i = 1; i < local_input.size(); i++)
    {
        // Add the previous element to the current element
        local_input[i] += local_input[i - 1];
        // If it is the last iteration, then save it in grandSum
        if (i == local_input.size() - 1)
        {
            grandSum = local_input[i];
        }
    }

    // Now gather the grandSum on process 0
    // Gather the grandSum on process 0 in a vector
    vector<int> grandSumArray(world_size);
    MPI_Gather(&grandSum, 1, MPI_INT, grandSumArray.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Now on process 0 perform a blelloch scan on the grandSumArray
    if (world_rank == 0)
    {
        blelloch_scan(grandSumArray);
    }

    int amtToAdd;
    // Scatter the grandSumArray to all processes
    MPI_Scatter(grandSumArray.data(), 1, MPI_INT, &amtToAdd, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Now on each process add the amtToAdd to each element of the local input vector
    for (size_t i = 0; i < local_input.size(); i++)
    {
        local_input[i] += amtToAdd;
    }

    // Now gather the local input vectors on process 0
    vector<int> final(world_size * chunk_size);
    MPI_Gather(local_input.data(), chunk_size, MPI_INT, final.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Stop the time using omp_get_wtime()
    double end = MPI_Wtime();

    // If the rank is 0, then print the time and validate the prefix sum
    if (world_rank == 0)
    {
        validatePrefixSum(input, final);

        // Print the time taken
        printf("Time taken: %lf\n", (end - start) * 1000);

        // Save the time to a CSV file
        ofstream output_file("output.csv", ios::app);
        output_file << (end - start) * 1000 << endl;
    }

    MPI_Finalize();

    return 0;
}