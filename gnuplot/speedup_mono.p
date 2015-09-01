#!/usr/bin/gnuplot

reset

#set terminal wxt size 800,600 dashed enhanced font 'Verdana,10' persist

set terminal epslatex size 14cm,11cm color colortext header \
"\\newcommand{\\ft}[0]{\\footnotesize}"

set output "mono.tex"

set grid xtics ytics ls 3 lw 1 lc rgb 'gray'

set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror out scale 0.75

set xlabel "Liczba wątków"
set ylabel "Przyspieszenie S" offset 2
set style fill solid 1.0 border rgb 'black'

set border linewidth 1.5 
set key left top
set style line 1 lc rgb "#355C7D" pt 6 ps 1 lt 1 lw 3 
set style line 2 lc rgb "#6C5B7B" pt 2 ps 1 lt 2 lw 3
set style line 3 lc rgb "#C06C84" pt 3 ps 1 lt 3 lw 3
set style line 4 lc rgb "#F67280" pt 4 ps 1 lt 4 lw 3
set style line 5 lc rgb "#F8B195" pt 5 ps 1 lt 5 lw 3

set style line 8 lt 8 lw 6 lc rgb 'black'

set yrange [0:*]
#set ytics 1,1
set ytics auto
set mytics 2
f(x) = mean_seq
fit f(x) 'sequential.dat' u 5:1 via mean_seq

plot 'omp.dat' u (mean_seq/$1):xticlabel(6) w linesp t 'OMP parfor' ls 1, \
'mkl.dat' u (mean_seq/$1):xticlabel(6) w linesp t 'MKL DGEMM' ls 2, \
1 ls 8 t 'Naiwny'

set output
set terminal dumb 
replot
