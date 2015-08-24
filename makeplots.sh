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

./parse.pl cannon | sort -t$'\t' -k5 -n > gnuplot/cannon.dat
./parse.pl mkl | sort -t$'\t' -k5 -n > gnuplot/mkl.dat
./parse.pl sequential | sort -t$'\t' -k5 -n > gnuplot/sequential.dat
./parse.pl cannon_omp | sort -t$'\t' -k5 -n > gnuplot/cannon_omp.dat
./parse.pl cannon_dgemm | sort -t$'\t' -k5 -n > gnuplot/cannon_dgemm.dat
./parse.pl omp | sort -t$'\t' -k5 -n > gnuplot/omp.dat

cd gnuplot
gnuplot ./speedup.p

cp speedup.eps speedup.tex ../paper/includes/plots/
