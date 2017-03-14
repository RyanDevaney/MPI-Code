#!/bin/bash

#SBATCH -n20
#SBATCH --time=00:5:00
#SBATCH --output=out/static.out

srun static p20 w5000 h5000
