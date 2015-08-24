#!/usr/bin/gnuplot

reset

#set terminal wxt size 800,600 dashed enhanced font 'Verdana,10' persist

set terminal epslatex size 15cm,7cm color colortext header \
"\\newcommand{\\ft}[0]{\\footnotesize}"

set output "speedup.tex"

set grid xtics ytics ls 3 lw 1 lc rgb 'gray'

set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror out scale 0.75

set xlabel "Liczba procesów"
set ylabel "Przyspieszenie S"
set style fill solid 1.0 border rgb 'black'

set border linewidth 1.5 
set style line 1 lc rgb 'black' pt 6 ps 2 lt 1 lw 4 
set style line 2 lt 3 lw 4 lc rgb 'black' 
set style line 3 lc rgb 'black' pt 1 ps 2 lt 2 lw 4
set style line 4 lc rgb 'black' pt 2 ps 2 lt 4 lw 4
set yrange [0:*]
set ytics 1,1
set mytics 2
f(x) = mean_seq
fit f(x) 'mkl.dat' u 5:1 via mean_seq

plot 'cannon.dat' u (mean_seq/$1):xticlabel(5) w linesp t 'Cannon' ls 1, \
1 ls 2 t 'MKL', \
'cannon_dgemm.dat' u (mean_seq/$1):xticlabel(5) w linesp t 'Cannon-DGEMM' ls 3, \
'cannon_omp.dat' u (mean_seq/$1):xticlabel(5) w linesp t 'Cannon-OpenMP' ls 4
set output
set terminal dumb 
replot
