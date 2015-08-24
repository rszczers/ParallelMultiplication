#!/bin/bash
./parse.pl cannon | sort -t$'\t' -k5 -n > gnuplot/cannon.dat
./parse.pl mkl | sort -t$'\t' -k5 -n > gnuplot/mkl.dat
./parse.pl sequential | sort -t$'\t' -k5 -n > gnuplot/sequential.dat
./parse.pl cannon_omp | sort -t$'\t' -k5 -n > gnuplot/cannon_omp.dat
./parse.pl cannon_dgemm | sort -t$'\t' -k5 -n > gnuplot/cannon_dgemm.dat
./parse.pl omp | sort -t$'\t' -k5 -n > gnuplot/omp.dat

cd gnuplot
gnuplot ./speedup.p

cp speedup.eps speedup.tex ../paper/includes/plots/
