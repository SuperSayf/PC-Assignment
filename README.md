# PC-Assignment

# Edit arguments inside run.sh

arg1: n, where n is 2^n (This is for automatic number generation)
arg2: run all the selected related programs (scan or bitonic)
arg3: run all the selected programs with selected input type (manual or automatic) (leave automatic)
arg4: number of processes for MPI

# Run with

`bash run.sh`

# For cluster:

Make sure to change directories for slurm outputs
`sbatch run_mscluster.slurm`
