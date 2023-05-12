#!/bin/bash

make clean

rm *.csv

# Compile the program
make

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Error: Compilation failed"
    exit 1
fi

# Title for start of number generation
echo "-----------------------Starting Number Generation-----------------------"

# Leave a blank line
echo ""

# Generate input data
./generator 25

# Title for end of number generation
echo "-----------------------Completed Number Generation-----------------------"

# Leave a blank line
echo ""

# Title for the first program
echo "-----------------------Running scan program-----------------------"

./scan

# Leave a blank line
echo ""

# Title for the second program
echo "-----------------------Running scan_omp program-----------------------"
./scan_omp