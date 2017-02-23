#!/bin/bash

#SBATCH --time=00:10:00
#SBATCH -N2
#SBATCH --output=two_box.out

srun pa01
