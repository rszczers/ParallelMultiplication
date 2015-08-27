#!/bin/bash
#PBS -N pmm
#PBS -l nodes=40:ppn=1
#PBS -l walltime=08:00:00
#PBS -q default 
#PBS -M rafal.szczerski@gmail.com
#PBS -m abe
#PBS -o pmm.log
export MV2_ENABLE_AFFINITY=0
#export KMP_AFFINITY=balanced
cd $PBS_O_WORKDIR
make test
