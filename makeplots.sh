#!/bin/bash
#rm ./debug/*
#export MV2_ENABLE_AFFINITY=0
i=4
for((n=4; n<=1024; i=i+2)); do
    export OMP_NUM_THREADS=1
    make cannon NPROC=$n OMP_THREADS=1 SIZE=$2
    echo "make cannon NPROC=$n OMP_THREADS=1 SIZE=$2"
    #n=$(echo $(($i*$i)))
    n=$(echo 2^$i | bc)
    sleep 1;
done

i=4;
for((n=4; n<=$1; i=i+2)); do
    for t in 1 2 4 12; do
        export OMP_NUM_THREADS=$t 
        make cannon_dgemm NPROC=$n OMP_THREADS=$t SIZE=$2
        echo "make cannon_dgemm NPROC=$n OMP_THREADS=$t SIZE=$2"
        sleep 1;
    done
    #n=$(($i*$i))
    n=$(echo 2^$i | bc)
done

i=4;
for((n=4; n<=$1; i=i+2)); do
    for t in 1 2 4 12; do
        export OMP_NUM_THREADS=$t
        make cannon_omp NPROC=$n OMP_THREADS=$t SIZE=$2
        echo "make cannon_omp NPROC=$n OMP_THREADS=$t SIZE=$2"   
        sleep 1;
    done
    #n=$(($i*$i))
    n=$(echo 2^$i | bc)
done

for((j=0; j<3; j++)); do
    make seq OMP_THREADS=1 NPROC=1 SIZE=$2
    sleep 1;
    echo "make seq OMP_THREADS=1 SIZE=$2"   
done

export MV2_ENABLE_AFFINITY=1
export KMP_AFFINITY=balanced
for((t=1; t<=72; t++)); do
    export OMP_NUM_THREADS=$t
    make mkl OMP_THREADS=$t SIZE=$2
    sleep 1;
    echo "make mkl OMP_THREADS=$t SIZE=$2"
done

for((t=1; t<=72; t++)); do
    export OMP_NUM_THREADS=$t
    make omp OMP_THREADS=$t SIZE=$2
    sleep 1;
    echo "make omp OMP_THREADS=$t SIZE=$2"
done

./data.sh
