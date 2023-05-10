#include <iostream>
#include <vector>

using namespace std;

void validatePrefixSum(const vector<int>& temp, const vector<int>& final) {
    int curr = 0;

    for (const auto& t : temp) {
        curr += t;

        if (final[&t - &temp[0]] != curr) {
            printf("INVALID PREFIX SUM\n");
            exit(1);
        }
    }

    printf("VALID PREFIX SUM\n");
}

void checkIfPowerOfTwo(int n) {
    // Check if the size of the input array is a power of two
    if (n == 0) {
        cout << "Not a power of two" << endl;
        exit(1);
    }
    
    while (n != 1) {
        if (n % 2 != 0) {
            cout << "Not a power of two" << endl;
            exit(1);
        }
        n = n / 2;
    }
}

void blelloch_scan(vector<int>& nums) {
    int n = nums.size();

    // Up-sweep (reduce) phase
    for (int d = 1; d < n; d *= 2) {
        for (int i = 0; i < n; i += 2*d) {
            nums[i+2*d-1] += nums[i+d-1];
        }
    }

    // Set root to 0
    nums[n-1] = 0;

    // Down-sweep (scan) phase
    for (int d = n/2; d > 0; d /= 2) {
        for (int i = 0; i < n; i += 2*d) {
            int t = nums[i+d-1];
            nums[i+d-1] = nums[i+2*d-1];
            nums[i+2*d-1] += t;
        }
    }
}

int main() {
    vector<int> nums = {2, 1, 4, 0, 3, 7, 6, 3};
    vector<int> temp = nums;

    // Check if the size of the input array is a power of two
    checkIfPowerOfTwo(nums.size());

    // Calculate the scan of the input array
    blelloch_scan(nums);

    // Calculate the prefix sum of the input array
    vector<int> final(nums.size());
    for (size_t i = 0; i < nums.size(); i++) {
        final[i] = nums[i] + temp[i];
    }

    // Check if the prefix sum is correct
    validatePrefixSum(temp, final);

    // Print the prefix sum
    for (size_t i = 0; i < final.size(); i++) {
        cout << final[i] << " ";
    }

    cout << endl;

    return 0;
}
