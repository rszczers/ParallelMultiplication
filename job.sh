#!/bin/sh
#PBS -N pmm
#PBS -l nodes=40:ppn=1
#PBS -l pmem=2gb
#PBS -l walltime=02:00:00
#PBS -q default 
#PBS -M rafal.szczerski@gmail.com
#PBS -m abe
#PBS -o pmm_0.log
cd /home/rszczers/ParallelMultiplication
make test
