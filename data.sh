#!/bin/bash
./parse.pl sequential ./debug/seq | sort -t$'\t' -k5 -n > gnuplot/data/scal_sequential.dat
./parse.pl cannon ./debug/seq | sort -t$'\t' -k5 -n > gnuplot/data/scal_cannon_seq.dat
./parse.pl cannon_omp ./debug/seq | sort -t$'\t' -k5 -n > gnuplot/data/scal_cannon_omp.dat
./parse.pl cannon_dgemm ./debug/seq | sort -t$'\t' -k5 -n > gnuplot/data/scal_cannon_dgemm.dat

cat gnuplot/data/scal_cannon_dgemm.dat | awk '$6==1' > gnuplot/data/scal_cannon_dgemm_1.dat
cat gnuplot/data/scal_cannon_dgemm.dat | awk '$6==2' > gnuplot/data/scal_cannon_dgemm_2.dat
cat gnuplot/data/scal_cannon_dgemm.dat | awk '$6==4' > gnuplot/data/scal_cannon_dgemm_4.dat
cat gnuplot/data/scal_cannon_dgemm.dat | awk '$6==12' > gnuplot/data/scal_cannon_dgemm_12.dat
cat gnuplot/data/scal_cannon_omp.dat | awk '$6==1' > gnuplot/data/scal_cannon_omp_1.dat
cat gnuplot/data/scal_cannon_omp.dat | awk '$6==2' > gnuplot/data/scal_cannon_omp_2.dat
cat gnuplot/data/scal_cannon_omp.dat | awk '$6==4' > gnuplot/data/scal_cannon_omp_4.dat
cat gnuplot/data/scal_cannon_omp.dat | awk '$6==12' > gnuplot/data/scal_cannon_omp_12.dat

# Data without matrix scaling goes down here
./parse.pl cannon ./debug | sort -t$'\t' -k5 -n | awk '$2 == 2048' > gnuplot/data/2048_cannon_seq.dat
./parse.pl cannon ./debug | sort -t$'\t' -k5 -n | awk '$2 == 4096' > gnuplot/data/4096_cannon_seq.dat
./parse.pl cannon ./debug | sort -t$'\t' -k5 -n | awk '$2 == 8192' > gnuplot/data/8192_cannon_seq.dat
./parse.pl cannon ./debug | sort -t$'\t' -k5 -n | awk '$2 == 16384' > gnuplot/data/16384_cannon_seq.dat

./parse.pl mkl ./debug | sort -t$'\t' -k6 -n | awk '$2 == 2048' > gnuplot/data/2048_mkl.dat
./parse.pl mkl ./debug | sort -t$'\t' -k6 -n | awk '$2 == 4096' > gnuplot/data/4096_mkl.dat
./parse.pl mkl ./debug | sort -t$'\t' -k6 -n | awk '$2 == 8192' > gnuplot/data/8192_mkl.dat
./parse.pl mkl ./debug | sort -t$'\t' -k6 -n | awk '$2 == 16384' > gnuplot/data/16384_mkl.dat

./parse.pl sequential ./debug | sort -t$'\t' -k5 -n | awk '$2 == 2048' > gnuplot/data/2048_sequential.dat
./parse.pl sequential ./debug | sort -t$'\t' -k5 -n | awk '$2 == 4096' > gnuplot/data/4096_sequential.dat
./parse.pl sequential ./debug | sort -t$'\t' -k5 -n | awk '$2 == 8192' > gnuplot/data/8192_sequential.dat
./parse.pl sequential ./debug | sort -t$'\t' -k5 -n | awk '$2 == 16384' > gnuplot/data/16384_sequential.dat

./parse.pl cannon_omp ./debug | sort -t$'\t' -k5 -n | awk '$2 == 2048' > gnuplot/data/2048_cannon_omp.dat
./parse.pl cannon_omp ./debug | sort -t$'\t' -k5 -n | awk '$2 == 4096' > gnuplot/data/4096_cannon_omp.dat
./parse.pl cannon_omp ./debug | sort -t$'\t' -k5 -n | awk '$2 == 8192' > gnuplot/data/8192_cannon_omp.dat
./parse.pl cannon_omp ./debug | sort -t$'\t' -k5 -n | awk '$2 == 16384' > gnuplot/data/16384_cannon_omp.dat

./parse.pl cannon_dgemm ./debug | sort -t$'\t' -k5 -n | awk '$2 == 2048' > gnuplot/data/2048_cannon_dgemm.dat
./parse.pl cannon_dgemm ./debug | sort -t$'\t' -k5 -n | awk '$2 == 4096' > gnuplot/data/4096_cannon_dgemm.dat
./parse.pl cannon_dgemm ./debug | sort -t$'\t' -k5 -n | awk '$2 == 8192' > gnuplot/data/8192_cannon_dgemm.dat
./parse.pl cannon_dgemm ./debug | sort -t$'\t' -k5 -n | awk '$2 == 16384' > gnuplot/data/16384_cannon_dgemm.dat

./parse.pl omp ./debug | sort -t$'\t' -k6 -n | awk '$2 == 2048' > gnuplot/data/2048_omp.dat
./parse.pl omp ./debug | sort -t$'\t' -k6 -n | awk '$2 == 4096' > gnuplot/data/4096_omp.dat
./parse.pl omp ./debug | sort -t$'\t' -k6 -n | awk '$2 == 8192' > gnuplot/data/8192_omp.dat
./parse.pl omp ./debug | sort -t$'\t' -k6 -n | awk '$2 == 16384' > gnuplot/data/16384_omp.dat

cat gnuplot/data/2048_cannon_dgemm.dat ./debug | awk '$6==1' > gnuplot/data/2048_cannon_dgemm_1.dat
cat gnuplot/data/2048_cannon_dgemm.dat ./debug | awk '$6==2' > gnuplot/data/2048_cannon_dgemm_2.dat
cat gnuplot/data/2048_cannon_dgemm.dat ./debug | awk '$6==4' > gnuplot/data/2048_cannon_dgemm_4.dat
cat gnuplot/data/2048_cannon_dgemm.dat ./debug | awk '$6==12' > gnuplot/data/2048_cannon_dgemm_12.dat

cat gnuplot/data/4096_cannon_dgemm.dat ./debug | awk '$6==1' > gnuplot/data/4096_cannon_dgemm_1.dat
cat gnuplot/data/4096_cannon_dgemm.dat ./debug | awk '$6==2' > gnuplot/data/4096_cannon_dgemm_2.dat
cat gnuplot/data/4096_cannon_dgemm.dat ./debug | awk '$6==4' > gnuplot/data/4096_cannon_dgemm_4.dat
cat gnuplot/data/4096_cannon_dgemm.dat ./debug | awk '$6==12' > gnuplot/data/4096_cannon_dgemm_12.dat

cat gnuplot/data/8192_cannon_dgemm.dat ./debug | awk '$6==1' > gnuplot/data/8192_cannon_dgemm_1.dat
cat gnuplot/data/8192_cannon_dgemm.dat ./debug | awk '$6==2' > gnuplot/data/8192_cannon_dgemm_2.dat
cat gnuplot/data/8192_cannon_dgemm.dat ./debug | awk '$6==4' > gnuplot/data/8192_cannon_dgemm_4.dat
cat gnuplot/data/8192_cannon_dgemm.dat ./debug | awk '$6==12' > gnuplot/data/8192_cannon_dgemm_12.dat

cat gnuplot/data/2048_cannon_omp.dat ./debug | awk '$6==1' > gnuplot/data/2048_cannon_omp_1.dat
cat gnuplot/data/2048_cannon_omp.dat ./debug | awk '$6==2' > gnuplot/data/2048_cannon_omp_2.dat
cat gnuplot/data/2048_cannon_omp.dat ./debug | awk '$6==4' > gnuplot/data/2048_cannon_omp_4.dat
cat gnuplot/data/2048_cannon_omp.dat ./debug | awk '$6==12' > gnuplot/data/2048_cannon_omp_12.dat

cat gnuplot/data/4096_cannon_omp.dat ./debug | awk '$6==1' > gnuplot/data/4096_cannon_omp_1.dat
cat gnuplot/data/4096_cannon_omp.dat ./debug | awk '$6==2' > gnuplot/data/4096_cannon_omp_2.dat
cat gnuplot/data/4096_cannon_omp.dat ./debug | awk '$6==4' > gnuplot/data/4096_cannon_omp_4.dat
cat gnuplot/data/4096_cannon_omp.dat ./debug | awk '$6==12' > gnuplot/data/4096_cannon_omp_12.dat

cat gnuplot/data/8192_cannon_omp.dat ./debug | awk '$6==1' > gnuplot/data/8192_cannon_omp_1.dat
cat gnuplot/data/8192_cannon_omp.dat ./debug | awk '$6==2' > gnuplot/data/8192_cannon_omp_2.dat
cat gnuplot/data/8192_cannon_omp.dat ./debug | awk '$6==4' > gnuplot/data/8192_cannon_omp_4.dat
cat gnuplot/data/8192_cannon_omp.dat ./debug | awk '$6==12' > gnuplot/data/8192_cannon_omp_12.dat


cd gnuplot
gnuplot ./speedup_cannon_seq_diff_n.p
gnuplot ./speedup_cannon_dgemm_diff_n.p
gnuplot ./speedup_cannon_dgemm.p
gnuplot ./speedup_mono.p
gnuplot ./speedup_cannon_omp.p
gnuplot ./speedup_cannon_dgemm_scal.p
gnuplot ./speedup_cannon_omp_scal.p

cp cannon_dgemm.eps cannon_dgemm.tex ../paper/includes/plots/
cp mono.eps mono.tex ../paper/includes/plots/
cp cannon_omp.eps cannon_omp.tex ../paper/includes/plots/
cp cannon_diff_n.eps cannon_diff_n.tex ../paper/includes/plots/
cp cannon_mkl_diff_n.eps cannon_mkl_diff_n.tex ../paper/includes/plots/
cp scal_cannon_dgemm.eps scal_cannon_dgemm.tex ../paper/includes/plots/
cp scal_cannon_omp.eps scal_cannon_omp.tex ../paper/includes/plots/

rm cannon_dgemm.eps cannon_dgemm.tex
rm mono.eps mono.tex
rm cannon_omp.eps cannon_omp.tex
rm cannon_diff_n.eps cannon_diff_n.tex
rm cannon_mkl_diff_n.eps cannon_mkl_diff_n.tex
rm scal_cannon_dgemm.eps scal_cannon_dgemm.tex
rm scal_cannon_omp.eps scal_cannon_omp.tex
rm fit.log
