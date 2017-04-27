#!/bin/bash

#SBATCH -n4
#SBATCH --tasks-per-node=8
#SBATCH --time=00:20:00
#SBATCH --open-mode=append
#SBATCH --output=parallel.out

srun parallel 4
