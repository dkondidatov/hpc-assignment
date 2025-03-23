#!/bin/sh
#SBATCH -p normal # partition (queue)
#SBATCH --time=2
#SBATCH --ntasks=2

#SBATCH -o /mnt/fsx/slurm/slurm.%N.%j.out # STDOUT
#SBATCH -e /mnt/fsx/slurm/slurm.%N.%j.err # STDERR

srun /mnt/fsx/slurm/pmm_v2