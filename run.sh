#!/bin/bash

# Arguments
# arg1: n, where n is 2^n

arg1=16

make clean

rm *.bin
rm *.csv

# Compile the program
make

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Error: Compilation failed"
    exit 1
fi

echo ""

# Title for start of number generation
echo "-----------------------Starting Number Generation----------------------"

# Leave a blank line
echo ""

# Generate input data
./generator $arg1

# Calculate 2^n
n=$((2**$arg1))

# Title for end of number generation
echo "Completed: Generated $n random numbers"

# Leave a blank line
echo ""

# # Title for the first program
# echo "-------------------------Running scan program--------------------------"

# ./scan

# # Leave a blank line
# echo ""

# # Title for the second program
# echo "------------------------Running scan_omp program-----------------------"

# ./scan_omp

# # Leave a blank line
# echo ""

# # Title for third program
# echo "------------------------Running scan_MPI program-----------------------"

# mpirun -np 2 ./scan_mpi

# # Leave a blank line
# echo ""

# # Title for speedup
# echo "---------------------------------Speedup-------------------------------"

# # Leave a blank line
# echo ""

# # Read the times from the output csv file, each time is in a new line in the same file
# line1=$(sed -n '1p' output.csv)
# line2=$(sed -n '2p' output.csv)
# line3=$(sed -n '3p' output.csv)

# # Output the times
# echo "Serial Time:   $line1 ms"
# echo "Parallel Time: $line2 ms"
# echo "MPI Time:      $line3 ms"

# # Calculate the speedup or speeddown based on the ratio of serial time to parallel time
# ratio=$(echo "scale=2; $line1/$line3" | bc)
# if [ $(echo "$ratio <= 1" | bc) -eq 1 ]; then
#     speeddown=$(echo "scale=2; $line3/$line1" | bc)
#     echo "Speed-up: INVALID, received speed-down of $speeddown"
# else
#     speedup=$(echo "scale=2; $line1/$line3" | bc)
#     echo "Speed-up: $speedup"
# fi

# echo ""

# Title for the third program
echo "-----------------------------Running bitonic program----------------------"

# Leave a blank line
echo ""

./bitonic

# Leave a blank line
echo ""

# Title for the fourth program
echo "----------------------------Running bitonic_omp program-------------------"

# Leave a blank line
echo ""

./bitonic_omp

# Leave a blank line
echo ""

# Title for the fifth program
echo "----------------------------Running bitonic_MPI program-------------------"

# Leave a blank line

mpirun -np 6 ./bitonic_mpi

# Leave a blank line
echo ""

# Title for speedup
echo "---------------------------------Speedup-------------------------------"

# Leave a blank line
echo ""

# Read the times from the output csv file, each time is in a new line in the same file
line1=$(sed -n '1p' output.csv)
line2=$(sed -n '2p' output.csv)
line3=$(sed -n '3p' output.csv)

# Output the times
echo "Serial Time:   $line1 ms"
echo "Parallel Time: $line2 ms"
echo "MPI Time:      $line3 ms"

# Calculate the speedup or speeddown based on the ratio of serial time to parallel time
ratio=$(echo "scale=2; $line1/$line3" | bc)
if [ $(echo "$ratio <= 1" | bc) -eq 1 ]; then
    speeddown=$(echo "scale=2; $line3/$line1" | bc)
    echo "Speed-up: INVALID, received speed-down of $speeddown"
else
    speedup=$(echo "scale=2; $line1/$line3" | bc)
    echo "Speed-up: $speedup"
fi