#!/bin/sh
#SBATCH -p normal # partition (queue)
#SBATCH --time=30
#SBATCH --ntasks=2
#SBATCH --chdir=/mnt/fsx/slurm

#SBATCH -o /mnt/fsx/slurm/slurm.%N.%j.out # STDOUT
#SBATCH -e /mnt/fsx/slurm/slurm.%N.%j.err # STDERR

srun /mnt/fsx/slurm/pmm_c_v2 /mnt/fsx/slurm/matrix_A.in /mnt/fsx/slurm/matrix_B.in /mnt/fsx/slurm/matrix_C.out