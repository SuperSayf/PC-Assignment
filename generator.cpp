#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " n" << endl;
        return 1;
    }

    int n = atoi(argv[1]);

    if (n < 0)
    {
        cerr << "Error: n must be non-negative." << endl;
        return 1;
    }

    int size = pow(2, n);
    int *arr = new int[size];
    srand(time(nullptr));

    // Initialize CSV file
    ofstream output_file("input.csv");

    // Initialize CSV header
    output_file << "input" << endl;

    for (int i = 0; i < size; i++)
    {
        arr[i] = rand() % 101; // generate random numbers between 0 and 100

        // Add to CSV file
        output_file << arr[i] << endl;
    }

    delete[] arr;

    return 0;
}
