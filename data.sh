#!/bin/bash
./parse.pl cannon | sort -t$'\t' -k5 -n | awk '$2 == 2048' > gnuplot/data/2048_cannon_seq.dat
./parse.pl cannon | sort -t$'\t' -k5 -n | awk '$2 == 4096' > gnuplot/data/4096_cannon_seq.dat
./parse.pl cannon | sort -t$'\t' -k5 -n | awk '$2 == 8192' > gnuplot/data/8192_cannon_seq.dat
./parse.pl cannon | sort -t$'\t' -k5 -n | awk '$2 == 16384' > gnuplot/data/16384_cannon_seq.dat

./parse.pl mkl | sort -t$'\t' -k6 -n | awk '$2 == 2048' > gnuplot/data/2048_mkl.dat
./parse.pl mkl | sort -t$'\t' -k6 -n | awk '$2 == 4096' > gnuplot/data/4096_mkl.dat
./parse.pl mkl | sort -t$'\t' -k6 -n | awk '$2 == 8192' > gnuplot/data/8192_mkl.dat
./parse.pl mkl | sort -t$'\t' -k6 -n | awk '$2 == 16384' > gnuplot/data/16384_mkl.dat

./parse.pl sequential | sort -t$'\t' -k5 -n | awk '$2 == 2048' > gnuplot/data/2048_sequential.dat
./parse.pl sequential | sort -t$'\t' -k5 -n | awk '$2 == 4096' > gnuplot/data/4096_sequential.dat
./parse.pl sequential | sort -t$'\t' -k5 -n | awk '$2 == 8192' > gnuplot/data/8192_sequential.dat
./parse.pl sequential | sort -t$'\t' -k5 -n | awk '$2 == 16384' > gnuplot/data/16384_sequential.dat

./parse.pl cannon_omp | sort -t$'\t' -k5 -n | awk '$2 == 2048' > gnuplot/data/2048_cannon_omp.dat
./parse.pl cannon_omp | sort -t$'\t' -k5 -n | awk '$2 == 4096' > gnuplot/data/4096_cannon_omp.dat
./parse.pl cannon_omp | sort -t$'\t' -k5 -n | awk '$2 == 8192' > gnuplot/data/8192_cannon_omp.dat
./parse.pl cannon_omp | sort -t$'\t' -k5 -n | awk '$2 == 16384' > gnuplot/data/16384_cannon_omp.dat

./parse.pl cannon_dgemm | sort -t$'\t' -k5 -n | awk '$2 == 2048' > gnuplot/data/2048_cannon_dgemm.dat
./parse.pl cannon_dgemm | sort -t$'\t' -k5 -n | awk '$2 == 4096' > gnuplot/data/4096_cannon_dgemm.dat
./parse.pl cannon_dgemm | sort -t$'\t' -k5 -n | awk '$2 == 8192' > gnuplot/data/8192_cannon_dgemm.dat
./parse.pl cannon_dgemm | sort -t$'\t' -k5 -n | awk '$2 == 16384' > gnuplot/data/16384_cannon_dgemm.dat

./parse.pl omp | sort -t$'\t' -k6 -n | awk '$2 == 2048' > gnuplot/data/2048_omp.dat
./parse.pl omp | sort -t$'\t' -k6 -n | awk '$2 == 4096' > gnuplot/data/4096_omp.dat
./parse.pl omp | sort -t$'\t' -k6 -n | awk '$2 == 8192' > gnuplot/data/8192_omp.dat
./parse.pl omp | sort -t$'\t' -k6 -n | awk '$2 == 16384' > gnuplot/data/16384_omp.dat

cat gnuplot/data/2048_cannon_dgemm.dat | awk '$6==1' > gnuplot/data/2048_cannon_dgemm_1.dat
cat gnuplot/data/2048_cannon_dgemm.dat | awk '$6==2' > gnuplot/data/2048_cannon_dgemm_2.dat
cat gnuplot/data/2048_cannon_dgemm.dat | awk '$6==4' > gnuplot/data/2048_cannon_dgemm_4.dat
cat gnuplot/data/2048_cannon_dgemm.dat | awk '$6==12' > gnuplot/data/2048_cannon_dgemm_12.dat

cat gnuplot/data/4096_cannon_dgemm.dat | awk '$6==1' > gnuplot/data/4096_cannon_dgemm_1.dat
cat gnuplot/data/4096_cannon_dgemm.dat | awk '$6==2' > gnuplot/data/4096_cannon_dgemm_2.dat
cat gnuplot/data/4096_cannon_dgemm.dat | awk '$6==4' > gnuplot/data/4096_cannon_dgemm_4.dat
cat gnuplot/data/4096_cannon_dgemm.dat | awk '$6==12' > gnuplot/data/4096_cannon_dgemm_12.dat

cat gnuplot/data/8192_cannon_dgemm.dat | awk '$6==1' > gnuplot/data/8192_cannon_dgemm_1.dat
cat gnuplot/data/8192_cannon_dgemm.dat | awk '$6==2' > gnuplot/data/8192_cannon_dgemm_2.dat
cat gnuplot/data/8192_cannon_dgemm.dat | awk '$6==4' > gnuplot/data/8192_cannon_dgemm_4.dat
cat gnuplot/data/8192_cannon_dgemm.dat | awk '$6==12' > gnuplot/data/8192_cannon_dgemm_12.dat

cat gnuplot/data/8192_cannon_omp.dat | awk '$6==1' > gnuplot/data/8192_cannon_omp_1.dat
cat gnuplot/data/8192_cannon_omp.dat | awk '$6==2' > gnuplot/data/8192_cannon_omp_2.dat
cat gnuplot/data/8192_cannon_omp.dat | awk '$6==4' > gnuplot/data/8192_cannon_omp_4.dat
cat gnuplot/data/8192_cannon_omp.dat | awk '$6==12' > gnuplot/data/8192_cannon_omp_12.dat


cd gnuplot
gnuplot ./speedup_cannon_seq_diff_n.p
gnuplot ./speedup_cannon_dgemm_diff_n.p
gnuplot ./speedup_cannon_dgemm.p
gnuplot ./speedup_mono.p
gnuplot ./speedup_cannon_omp.p

cp cannon_dgemm.eps cannon_dgemm.tex ../paper/includes/plots/
cp mono.eps mono.tex ../paper/includes/plots/
cp cannon_omp.eps cannon_omp.tex ../paper/includes/plots/
cp cannon_diff_n.eps cannon_diff_n.tex ../paper/includes/plots/
cp cannon_mkl_diff_n.eps cannon_mkl_diff_n.tex ../paper/includes/plots/