#!/bin/bash

# specify a partition
#SBATCH -p bigbatch
# specify number of nodes
#SBATCH -N 4
#SBATCH --exclude=mscluster[45-49]
# specify number of tasks
#SBATCH --ntasks=16
# specify number of cores (or threads) per process
##SBATCH -c 4
# specify memory pool per node (for all cores on the node)
##SBATCH --mem 10
# specify the wall clock time limit for the job (hh:mm:ss) 5 mins
##SBATCH -t 10:00:00
# specify the job name
#SBATCH -J scan
# specify the filename to be used for writing output 
# change the path for your own account here
#SBATCH -o /home-mscluster/sjumoorty2/PC-Assignment/mscluster_runs/scan/%N.%j.out
# specify the filename for stderr
# change the path for your own account here
#SBATCH -e /home-mscluster/sjumoorty2/PC-Assignment/mscluster_runs/scan/%N.%j.err

export LD_LIBRARY_PATH=/usr/local/mpich-4.1.1/lib:$LD_LIBRARY_PATH
export PATH=/usr/local/mpich-4.1.1/bin:$PATH

export PATH=/home-mscluster/sjumoorty2/openmpi/openmpi-4.1.4/build/bin:$PATH
export LD_LIBRARY_PATH=/home-mscluster/sjumoorty2/openmpi/openmpi-4.1.4/build/lib:$LD_LIBRARY_PATH

# Arguments
# arg1: n, where n is 2^n (This is for automatic number generation)
# arg2: run all the selected related programs (scan or bitonic)
# arg3: run all the selected programs with selected input type (manual or automatic)
# arg4: number of threads for MPI

arg1=16
arg2="scan"
arg3="automatic"
arg4=2

# Number generation
if [ "$arg3" = "automatic" ]; then

    # Clean the directory
    make clean

    # Remove the old files
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
    echo "------------------------------------------------------------------------"
    echo ""  
fi

if [ "$arg2" = "scan" ]; then

    # Go into the relevant directory
    cd scan

    # Clean the directory
    make clean

    # Remove the old files
    rm *.csv

    # Compile the program
    make

    # Check if the compilation was successful
    if [ $? -ne 0 ]; then
        echo "Error: Compilation failed"
        exit 1
    fi

    # Leave a blank line
    echo ""

    # Title for the first program
    echo "-------------------------Running scan program--------------------------"

    ./scan

    # Leave a blank line
    echo ""

    # Title for the second program
    echo "------------------------Running scan_omp program-----------------------"

    ./scan_omp

    # Leave a blank line
    echo ""

    # Title for third program
    echo "------------------------Running scan_mpi program-----------------------"

    # Leave a blank line
    echo ""

    mpirun -np $arg4 ./scan_mpi

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

    # Define the column widths
    col_width=25

    # Output the header
    printf "%-${col_width}s %-${col_width}s\n" "Description" "Time (ms)"
    printf "%-${col_width}s %-${col_width}s\n" "-----------" "---------"

    # Output the lines
    printf "%-${col_width}s %-${col_width}s\n" "Serial Time:" "$line1 ms"
    printf "%-${col_width}s %-${col_width}s\n" "Parallel Time:" "$line2 ms"
    printf "%-${col_width}s %-${col_width}s\n" "MPI Time (np=$arg4):" "$line3 ms"

    echo ""

    col_width=35

    # Calculate the speedup or speeddown between the serial and parallel times
    ratio=$(echo "scale=2; $line1/$line2" | bc)
    if [ $(echo "$ratio <= 1" | bc) -eq 1 ]; then
        speeddown=$(echo "scale=2; $line2/$line1" | bc)
        printf "%-${col_width}s %-${col_width}s\n" "Speed-down (Serial vs Parallel):" "INVALID, received speed-down of $speeddown"
    else
        speedup=$(echo "scale=2; $line1/$line2" | bc)
        printf "%-${col_width}s %-${col_width}s\n" "Speed-up (Serial vs Parallel):" "$speedup"
    fi

    # Calculate the speedup or speeddown between the serial and MPI times
    ratio=$(echo "scale=2; $line1/$line3" | bc)
    if [ $(echo "$ratio <= 1" | bc) -eq 1 ]; then
        speeddown=$(echo "scale=2; $line3/$line1" | bc)
        printf "%-${col_width}s %-${col_width}s\n" "Speed-down (Serial vs MPI):" "INVALID, received speed-down of $speeddown"
    else
        speedup=$(echo "scale=2; $line1/$line3" | bc)
        printf "%-${col_width}s %-${col_width}s\n" "Speed-up (Serial vs MPI):" "$speedup"
    fi

    echo ""
fi

if [ "$arg2" = "bitonic" ]; then

    # Go into the relevant directory
    cd bitonic

    # Clean the directory
    make clean

    # Remove the old files
    rm *.csv

    # Compile the program
    make

    # Check if the compilation was successful
    if [ $? -ne 0 ]; then
        echo "Error: Compilation failed"
        exit 1
    fi

    # Leave a blank line
    echo ""

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
    echo "----------------------------Running bitonic_mpi program-------------------"

    # Leave a blank line
    echo ""

    # Leave a blank line

    mpirun -np $arg4 ./bitonic_mpi

    # Leave a blank line
    echo ""

    # Title for speedup
    echo "-------------------------------------Speedup-------------------------------"

    # Leave a blank line
    echo ""

    # Read the times from the output csv file, each time is in a new line in the same file
    line1=$(sed -n '1p' output.csv)
    line2=$(sed -n '2p' output.csv)
    line3=$(sed -n '3p' output.csv)

    # Define the column widths
    col_width=25

    # Output the header
    printf "%-${col_width}s %-${col_width}s\n" "Description" "Time (ms)"
    printf "%-${col_width}s %-${col_width}s\n" "-----------" "---------"

    # Output the lines
    printf "%-${col_width}s %-${col_width}s\n" "Serial Time:" "$line1 ms"
    printf "%-${col_width}s %-${col_width}s\n" "Parallel Time:" "$line2 ms"
    printf "%-${col_width}s %-${col_width}s\n" "MPI Time (np=$arg4):" "$line3 ms"

    echo ""

    col_width=30

    # Calculate the speedup or speeddown between the serial and parallel times
    ratio=$(echo "scale=2; $line1/$line2" | bc)
    if [ $(echo "$ratio <= 1" | bc) -eq 1 ]; then
        speeddown=$(echo "scale=2; $line2/$line1" | bc)
        printf "%-${col_width}s %-${col_width}s\n" "Speed-down (Serial vs Parallel):" "INVALID, received speed-down of $speeddown"
    else
        speedup=$(echo "scale=2; $line1/$line2" | bc)
        printf "%-${col_width}s %-${col_width}s\n" "Speed-up (Serial vs Parallel):" "$speedup"
    fi

    # Calculate the speedup or speeddown between the serial and MPI times
    ratio=$(echo "scale=2; $line1/$line3" | bc)
    if [ $(echo "$ratio <= 1" | bc) -eq 1 ]; then
        speeddown=$(echo "scale=2; $line3/$line1" | bc)
        printf "%-${col_width}s %-${col_width}s\n" "Speed-down (Serial vs MPI):" "INVALID, received speed-down of $speeddown"
    else
        speedup=$(echo "scale=2; $line1/$line3" | bc)
        printf "%-${col_width}s %-${col_width}s\n" "Speed-up (Serial vs MPI):" "$speedup"
    fi

    echo ""
fi
