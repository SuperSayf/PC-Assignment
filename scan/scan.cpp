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

void scan(vector<int> &out, const vector<int> &in)
{
    int N = in.size();
    out.resize(N);

    if (N > 0)
    {
        out[0] = in[0];
    }

    for (int i = 1; i < N; i++)
    {
        out[i] = in[i] + out[i - 1];
    }
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
    vector<int> prefix_sum;
    scan(prefix_sum, nums);

    // Stop the timer
    double end = omp_get_wtime();

    // Print the prefix sum
    // for (size_t i = 0; i < prefix_sum.size(); i++) {
    //     cout << prefix_sum[i] << " ";
    // }

    cout << endl;

    // Print the time taken in ms
    cout << "Time taken: " << (end - start) * 1000 << " ms" << endl;

    // Save the time to a CSV file
    ofstream output_file("output.csv", ios::app);
    output_file << (end - start) * 1000 << endl;

    // Check if the prefix sum is correct
    validatePrefixSum(temp, prefix_sum);

    return 0;
}
