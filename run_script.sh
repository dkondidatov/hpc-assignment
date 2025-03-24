#!/bin/sh
#SBATCH -p normal # partition (queue)
#SBATCH --time=30
#SBATCH --nodes=3
#SBATCH --ntasks=6
#SBATCH --cpus-per-task=1
#SBATCH --chdir=/mnt/fsx/slurm

#SBATCH -o /mnt/fsx/slurm/slurm.%N.%j.out # STDOUT
#SBATCH -e /mnt/fsx/slurm/slurm.%N.%j.err # STDERR

export OMPI_MCA_btl_tcp_if_include=ens5
srun --mpi=pmix /mnt/fsx/slurm/matrix_mult_cpp /mnt/fsx/slurm/matrix_A.in /mnt/fsx/slurm/matrix_B.in /mnt/fsx/slurm/matrix_C.out


