#!/bin/bash

#SBATCH -N2
#SBATCH --output=timings.out
#SBATCH --time=00:20:00

srun timings
