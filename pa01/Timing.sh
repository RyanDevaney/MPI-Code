#!/bin/bash

#SBATCH -N2
#SBATCH --output=timings.out

srun timings
