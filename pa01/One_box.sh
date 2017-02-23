#!/bin/bash

#SBATCH --time=00:01:00
#SBATCH -n2
#SBATCH --output=one_box.out

srun pa01
