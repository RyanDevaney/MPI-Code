#!/bin/bash

#SBATCH -N2
#SBATCH --output=two_box.out
#SBATCH --time=00:10:00

srun pa01
