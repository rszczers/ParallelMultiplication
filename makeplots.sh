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
    for((t=1; t<=24; t++)); do
        export OMP_NUM_THREADS=$t
        make cannon_dgemm NPROC=$n OMP_THREADS=$t
        echo "make cannon_dgemm NPROC=$n"   
        n=$(($i*$i))
        echo $n;
        sleep 1;
    done
done

i=3;
for((n=4; n<=$1; i++)); do
    for((t=1; t<=24; t++)); do
        export OMP_NUM_THREADS=$t
        make cannon_omp NPROC=$n OMP_THREADS=$t
        echo "make cannon_omp NPROC=$n"   
        n=$(($i*$i))
        echo $n;
        sleep 1;
    done
done

for((j=0; j<3; j++)); do
    make seq
    sleep 1;
    echo "make seq"   
done

for((j=0; j<3; j++)); do
    for((t=1; t<=24; t++)); do
        export OMP_NUM_THREADS=$t
        make mkl OMP_THREADS=$t
        sleep 1;
        echo "make mkl"
    done
done

for((j=0; j<3; j++)); do
    for((t=1; t<=24; t++)); do
        export OMP_NUM_THREADS=$t
        make omp OMP_THREADS=$t
        sleep 1;
        echo "make omp"
    done
done

./data.sh
