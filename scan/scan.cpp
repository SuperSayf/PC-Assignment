#include <iostream>
#include <vector>
#include <omp.h>
#include <fstream>
#include <limits>

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
        last = nums[n - 1];
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

    // Increase the size of the array by 1 and set the last element to the last value of the up-sweep phase
    nums.resize(n + 1);
    nums[n] = last;

    // Remove the first element of the array
    nums.erase(nums.begin());
}

int main()
{
    // Read input from bin file
    vector<int> nums;
    int num;
    ifstream input_file("../input.bin", ios::binary);

    while (input_file.read(reinterpret_cast<char *>(&num), sizeof(int)))
    {
        nums.push_back(num);
    }

    vector<int> temp = nums;

    // Check if the size of the input array is a power of two
    checkIfPowerOfTwo(nums.size());

    // Start the timer using omp_get_wtime()
    double start = omp_get_wtime();

    // Calculate the scan of the input array
    blelloch_scan(nums);

    // Stop the timer
    double end = omp_get_wtime();

    // Print the prefix sum
    // for (size_t i = 0; i < nums.size(); i++)
    // {
    //     cout << nums[i] << " ";
    // }

    cout << endl;

    // Print the time taken in ms
    cout << "Time taken: " << (end - start) * 1000 << " ms" << endl;

    // Save the time to a CSV file
    ofstream output_file("output.csv", ios::app);
    output_file << (end - start) * 1000 << endl;

    // Check if the prefix sum is correct
    validatePrefixSum(temp, nums);

    return 0;
}
