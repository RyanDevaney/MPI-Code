#!/bin/bash

#SBATCH -n1
#SBATCH --time=00:7:00
#SBATCH --output=out/sequential.out

srun sequential w5000 h5000
