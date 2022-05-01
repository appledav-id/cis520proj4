#!/bin/bash -l
for i in {1..20}
do
    sbatch --ntasks-per-node=$i sbatch_script.sh $i $j
done