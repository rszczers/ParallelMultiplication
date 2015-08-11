#!/bin/bash
#rm ./debug/*
i=3;
for((n=4; n<=$1; i++)); do
    make cannon NPROC=$n
    sleep 1;
    echo "make cannon NPROC=$n"   
    n=$((i*i))
done

#for((j=0; j<3; j++)); do
#    make seq
#    sleep 1;
#    echo "make seq"   
#done

for((j=0; j<3; j++)); do
    make mkl 
    sleep 1;
    echo "make mkl"   
done

./parse.pl cannon | sort -t$'\t' -k5 -n > gnuplot/cannon.dat
./parse.pl mkl | sort -t$'\t' -k5 -n > gnuplot/mkl.dat
./parse.pl sequential | sort -t$'\t' -k5 -n > gnuplot/sequential.dat

cd gnuplot
./speedup.p
