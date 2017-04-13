#!/bin/bash

#SBATCH -n1
#SBATCH --time=00:20:00
#SBATCH --open-mode=append
#SBATCH --output=sequential.out

srun sequential 120
