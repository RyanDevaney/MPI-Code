#!/bin/bash

#SBATCH -n2
#SBATCH --output=one_box.out
#SBATCH --time=00:01:00


srun pa01
