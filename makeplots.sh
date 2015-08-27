#!/bin/bash
#rm ./debug/*
i=3;
for((n=4; n<=$1; i++)); do
    export OMP_NUM_THREADS=1
    make cannon NPROC=$n OMP_THREADS=1
    echo "make cannon NPROC=$n OMP_THREADS=1"
    n=$(($i*$i))
    echo $n;
    sleep 1;
done

i=3;
for((n=4; n<=$1; i++)); do
    for((t=1; t<=24; t++)); do
        export OMP_NUM_THREADS=$t
        make cannon_dgemm NPROC=$n OMP_THREADS=$t
        echo "make cannon_dgemm NPROC=$n OMP_THREADS=$t"   
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
        echo "make cannon_omp NPROC=$n OMP_THREADS=$t"   
        n=$(($i*$i))
        echo $n;
        sleep 1;
    done
done

for((j=0; j<3; j++)); do
    make seq OMP_THREADS=1 NPROC=1
    sleep 1;
    echo "make seq OMP_THREADS=1"   
   
done

for((t=1; t<=24; t++)); do
    export OMP_NUM_THREADS=$t
    make mkl OMP_THREADS=$t
    sleep 1;
    echo "make mkl OMP_THREADS=$t"
done

for((t=1; t<=24; t++)); do
    export OMP_NUM_THREADS=$t
    make omp OMP_THREADS=$t
    sleep 1;
    echo "make omp OMP_THREADS=$t"
done

./data.sh
