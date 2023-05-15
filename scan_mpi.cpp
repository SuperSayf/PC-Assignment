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

            // for (const auto &t : final)
            // {
            //     cout << t << " ";
            // }
            // cout << endl;
            exit(1);
        }
    }

    printf("VALID PREFIX SUM\n");

    // for (const auto &t : final)
    // {
    //     cout << t << " ";
    // }
    // cout << endl;
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
            if (i + d < n)
            {
                nums[i + 2 * d - 1] += nums[i + d - 1];
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    last = nums[n - 1];

    // Set root to 0
    nums[n - 1] = 0;

    // Down-sweep (scan) phase
    for (int d = n / 2; d > 0; d /= 2)
    {
        for (int i = 0; i < n; i += 2 * d)
        {
            if (i + d < n)
            {
                int t = nums[i + d - 1];
                nums[i + d - 1] = nums[i + 2 * d - 1];
                nums[i + 2 * d - 1] += t;
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
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

    // Read input from input.bin file on rank 0
    vector<int> nums;
    vector<int> temp;
    if (rank == 0)
    {
        int num;
        ifstream input_file("input.bin", ios::binary);

        while (input_file.read(reinterpret_cast<char *>(&num), sizeof(int)))
        {
            nums.push_back(num);
        }
        // nums = {2, 1, 4, 0, 3, 7, 6, 3};
        temp = nums;
    }

    // Broadcast the size of the input array to all processes
    int n = nums.size();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Check if the size of the input array is a power of two if the rank is 0
    if (rank == 0)
    {
        checkIfPowerOfTwo(n);
    }

    // Broadcast the input array to all processes
    nums.resize(n);
    MPI_Bcast(&nums[0], n, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform the scan operation
    double start_time = MPI_Wtime(); // Get the current time
    blelloch_scan(nums);
    double end_time = MPI_Wtime(); // Get the current time again

    // Validate the prefix sum
    if (rank == 0)
    {
        validatePrefixSum(temp, nums);
        // Print the time taken in ms
        cout << "Time taken: " << (end_time - start_time) * 1000 << " ms" << endl;
        ofstream output_file("output.csv", ios::app);
        output_file << (end_time - start_time) * 1000 << endl;
    }

    MPI_Finalize();

    return 0;
}
