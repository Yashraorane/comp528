#!/bin/bash -l
 
# Set the name of the job
#SBATCH --job-name=cheapyInsertion
 
# Specify the current working directory as the location for executables/files
#SBATCH -D ./
 
# Export the current environment to the compute node
#SBATCH --export=ALL
 
# Specific course queue, exclusive use (for timings), max 1 min wallclock time
#SBATCH -p course
#SBATCH -t 00:10:00
#SBATCH --exclusive
 
# Load the necessary modules
module load compilers/intel/2019u5
 
echo "Number of threads or processes : $SLURM_NTASKS"
 
# Compile and run the program with varying number of threads
# gcc -fopenmp -std=c99 -O3 cInsertion.c coordReader.c -o ci.exe -lm
 gcc -fopenmp -std=c99 -O3 fInsertion.c coordReader.c -o fi.exe -lm
# gcc -fopenmp -std=c99 -O3 ompcInsertion.c coordReader.c -o comp.exe -lm
# gcc -fopenmp -std=c99 -O3 ompfInsertion.c coordReader.c -o fomp.exe -lm
# icc -qopenmp -std=c99 -O3 ompcInsertion.c coordReader.c -o icomp.exe -lm
# icc -qopenmp -std=c99 -O3 ompfInsertion.c coordReader.c -o ifomp.exe -lm

# icc -qopenmp -std=c99 -O3 ompcInsertion.c coordReader.c -o icomp.exe -lm

 
# num_threads=(32)
num_iterations=1
 
# if '-c' not used then default to 1. SLURM_CPUS_PER_TASK is given by -c
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1}
 
#echo using ${OMP_NUM_THREADS} OpenMP threads
 
for ((i=1; i<=$num_iterations; i++)); do
    echo "Running iteration $i"
 
    time ./fi.exe "4096_coords.coord" "coutput.txt"
 
    echo '-------'
done