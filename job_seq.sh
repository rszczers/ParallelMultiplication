#!/bin/bash
#PBS -N pmm_seqpart
#PBS -l nodes=8
#PBS -l walltime=04:00:00
#PBS -q default 
#PBS -m abe
#PBS -M rafal.szczerski@gmail.com
#PBS -o pmm.log

cd $PBS_O_WORKDIR
make data SIZE=4096 
echo "make data SIZE=4096"
./makeplots_seq.sh 384 4096
mv ./debug/debug_* ./debug/seq/