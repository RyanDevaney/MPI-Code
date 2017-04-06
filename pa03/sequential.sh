#!/bin/bash

#SBATCH -n1
#SBATCH --time=00:5:00
#SBATCH --output=sequential.out

srun sequential data.txt
