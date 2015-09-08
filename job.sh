#!/bin/bash
#PBS -N pmm
#PBS -l nodes=14:ppn=1,mem=8GB,walltime=04:00:00:00
#PBS -q default 
#PBS -m abe
#PBS -M rafal.szczerski@gmail.com
#PBS -o pmm.log
cd $PBS_O_WORKDIR

for s in 2048 4096 8192; do
    make data SIZE=$s
    echo "make data SIZE=$s"
    sleep 1
    make test SIZE=$s
    mv ./debug/debug_* ./debug/tmp/
done


