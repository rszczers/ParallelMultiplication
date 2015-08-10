#!/bin/bash
i=3;
for((n=4; n<=$1; i++)); do
    make cannon NPROC=$n
    sleep 1;
    echo "make cannon NPROC=$n"   
    n=$((i*i))
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

./analyse.pl cannon | sort -k1,8 > gnuplot/cannon.dat
./analyse.pl mkl | sort -k1,8 > gnuplot/mkl.dat
./analyse.pl sequential | sort -k1,8 > gnuplot/sequential.dat

cd gnuplot
gnuplot speedup.p
