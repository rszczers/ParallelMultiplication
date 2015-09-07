#!/bin/bash
#PBS -N pmm
#PBS -l nodes=32
#PBS -l walltime=72:00:00
#PBS -q default 
#PBS -M rafal.szczerski@gmail.com
#PBS -m abe
#PBS -o pmm.log
#export MV2_ENABLE_AFFINITY=0
#export KMP_AFFINITY=balanced
cd $PBS_O_WORKDIR

for s in 2048 4096 8192 16384; do
    make data SIZE=$s
    echo "make data SIZE=$s"
    sleep 1
    make test SIZE=$s
    mv ./debug/debug* ./debug/job2/
done
