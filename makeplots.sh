#!/bin/bash
#rm ./debug/*
i=3;
for((n=4; n<=$1; i++)); do
    make cannon NPROC=$n
    echo "make cannon NPROC=$n"   
    n=$(($i*$i))
    echo $n;
    sleep 1;
done

i=3;
for((n=4; n<=$1; i++)); do
    make cannon_dgemm NPROC=$n
    echo "make cannon_dgemm NPROC=$n"   
    n=$(($i*$i))
    echo $n;
    sleep 1;
done

i=3;
for((n=4; n<=$1; i++)); do
    make cannon_omp NPROC=$n
    echo "make cannon_omp NPROC=$n"   
    n=$(($i*$i))
    echo $n;
    sleep 1;
done

for((j=0; j<3; j++)); do
    make seq
    sleep 1;
    echo "make seq"   
done

for((j=0; j<3; j++)); do
    make mkl 
    sleep 1;
    echo "make mkl"   
done

for((j=0; j<3; j++)); do
    make omp
    sleep 1;
    echo "make omp"   
done

./data.sh
