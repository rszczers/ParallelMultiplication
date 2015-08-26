#!/bin/bash
./parse.pl cannon | sort -t$'\t' -k5 -n > gnuplot/cannon.dat
./parse.pl mkl | sort -t$'\t' -k5 -n > gnuplot/mkl.dat
./parse.pl sequential | sort -t$'\t' -k5 -n > gnuplot/sequential.dat
./parse.pl cannon_omp | sort -t$'\t' -k5 -n > gnuplot/cannon_omp.dat
./parse.pl cannon_dgemm | sort -t$'\t' -k5 -n > gnuplot/cannon_dgemm.dat
./parse.pl omp | sort -t$'\t' -k5 -n > gnuplot/omp.dat

cat gnuplot/cannon_dgemm.dat | awk '$6==1' > gnuplot/cannon_dgemm_1.dat
cat gnuplot/cannon_dgemm.dat | awk '$6==2' > gnuplot/cannon_dgemm_2.dat
cat gnuplot/cannon_dgemm.dat | awk '$6==4' > gnuplot/cannon_dgemm_4.dat
cat gnuplot/cannon_dgemm.dat | awk '$6==8' > gnuplot/cannon_dgemm_8.dat
cat gnuplot/cannon_dgemm.dat | awk '$6==12' > gnuplot/cannon_dgemm_12.dat
cat gnuplot/cannon_dgemm.dat | awk '$6==16' > gnuplot/cannon_dgemm_16.dat
cat gnuplot/cannon_dgemm.dat | awk '$6==20' > gnuplot/cannon_dgemm_20.dat
cat gnuplot/cannon_dgemm.dat | awk '$6==24' > gnuplot/cannon_dgemm_24.dat

cat gnuplot/cannon_omp.dat | awk '$6==1' > gnuplot/cannon_omp_1.dat
cat gnuplot/cannon_omp.dat | awk '$6==2' > gnuplot/cannon_omp_2.dat
cat gnuplot/cannon_omp.dat | awk '$6==4' > gnuplot/cannon_omp_4.dat
cat gnuplot/cannon_omp.dat | awk '$6==8' > gnuplot/cannon_omp_8.dat
cat gnuplot/cannon_omp.dat | awk '$6==12' > gnuplot/cannon_omp_12.dat
cat gnuplot/cannon_omp.dat | awk '$6==16' > gnuplot/cannon_omp_16.dat
cat gnuplot/cannon_omp.dat | awk '$6==20' > gnuplot/cannon_omp_20.dat
cat gnuplot/cannon_omp.dat | awk '$6==24' > gnuplot/cannon_omp_24.dat


cd gnuplot
#gnuplot ./speedup.p

#cp speedup.eps speedup.tex ../paper/includes/plots/
