#!/usr/bin/gnuplot

reset

#set terminal wxt size 800,600 dashed enhanced font 'Verdana,10' persist

set terminal epslatex size 14cm,10cm  color colortext header \
"\\newcommand{\\ft}[0]{\\footnotesize}"

set output "scal_cannon_omp.tex"

set grid xtics ytics ls 3 lw 1 lc rgb 'gray'

set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror out scale 0.75

set xlabel "Liczba procesów"
set ylabel "Przyspieszenie" offset 2
set style fill solid 1.0 border rgb 'black'

set border linewidth 1.5 
set key left top
set key width -6

set style line 1 lc rgb "#355C7D" pt 6 ps 2 lt 1 lw 2 
set style line 2 lc rgb "#6C5B7B" pt 2 ps 2 lt 2 lw 2
set style line 3 lc rgb "#C06C84" pt 3 ps 2 lt 3 lw 2
set style line 4 lc rgb "#F67280" pt 4 ps 2 lt 4 lw 2
set style line 5 lc rgb "#F8B195" pt 7 ps 1 lt 5 lw 2
set title "Hybrydowy algorytm Cannona"
set style line 8 lt 8 lw 6 lc rgb 'black'

set yrange [0:16]
set xrange [*:256]
#set ytics 1,1
set ytics auto
set mytics 2
f(x) = mean_seq
fit f(x) 'data/scal_sequential.dat' u 5:1 via mean_seq

set label "Algorytm naiwny" at  196, 1.7

#set logscale x
plot 'data/scal_cannon_omp_1.dat' u 5:(mean_seq/$1):xticlabel(5) w linesp t 'Cannon-OMP, 1 wątek' ls 1, \
'data/scal_cannon_omp_4.dat' u 5:(mean_seq/$1) w linesp t 'Cannon-OMP, 4 wątków' ls 2, \
'data/scal_cannon_omp_12.dat' u 5:(mean_seq/$1) w linesp t 'Cannon-OMP, 12 wątków' ls 3, \
'data/scal_cannon_seq.dat' u 5:(mean_seq/$1) w linesp t 'Cannon' ls 5, \
1 ls 8 notitle

set output
set terminal dumb 
replot