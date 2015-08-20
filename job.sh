#!/bin/bash
#PBS -N pmm
#PBS -l nodes=20:ppn=2
#PBS -l walltime=02:00:00
#PBS -q default 
#PBS -M rafal.szczerski@gmail.com
#PBS -m abe
#PBS -o pmm.log

cd $PBS_O_WORKDIR
#cd /home/rszczers/ParallelMultiplication/
make test
