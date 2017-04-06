#!/bin/bash

#SBATCH -N2
#SBATCH -n8
#SBATCH --time=00:5:00
#SBATCH --output=parallel.out

srun parallel data.txt 
