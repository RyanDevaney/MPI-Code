#!/bin/bash

#SBATCH --time=00:20:00
#SBATCH -N2
#SBATCH --output=timings.out

srun timings
