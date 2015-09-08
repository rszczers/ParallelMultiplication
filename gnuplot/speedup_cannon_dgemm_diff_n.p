#!/usr/bin/gnuplot

reset

#set terminal wxt size 800,600 dashed enhanced font 'Verdana,10' persist

set terminal epslatex size 15cm,12cm color colortext header \
"\\newcommand{\\ft}[0]{\\footnotesize}"

set output "cannon_mkl_diff_n.tex"


set multiplot layout 2,2 title "Hybrydowy algorytm Cannona"

set grid xtics ytics ls 3 lw 1 lc rgb 'gray'

set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror out scale 0.75


set style fill solid 1.0 border rgb 'black'

set border linewidth 1.5 

set style line 1 lc rgb "#355C7D" pt 6 ps 2 lt 1 lw 1 
set style line 2 lc rgb "#6C5B7B" pt 2 ps 2 lt 1 lw 1
set style line 3 lc rgb "#C06C84" pt 3 ps 2 lt 1 lw 1
set style line 4 lc rgb "#F67280" pt 4 ps 2 lt 1 lw 1
set style line 5 lc rgb "#F8B195" pt 5 ps 2 lt 1 lw 1

set style line 8 lt 8 lw 6 lc rgb 'black'

set yrange [0:55]
#set ytics 1,1
set ytics auto
set mytics 2

set boxwidth 0.25
set style fill solid
set logscale x
f(x) = mean_seq_2048
g(x) = mean_seq_4096
h(x) = mean_seq_8192

set key right top

fit f(x) 'data/2048_sequential.dat' u 5:1 via mean_seq_2048
fit g(x) 'data/4096_sequential.dat' u 5:1 via mean_seq_4096
fit h(x) 'data/8192_sequential.dat' u 5:1 via mean_seq_8192

set style fill pattern border


set ylabel "Przyspieszenie" offset 1
set key off
set title "1 wątek"
plot 'data/8192_cannon_dgemm_1.dat' u 5:(mean_seq_8192/$1):xtic(5) w boxes ls 2 lt -1 t '$n = 8192\times 8192$', \
'data/4096_cannon_dgemm_1.dat' u ($5*1.25):(mean_seq_4096/$1) w boxes ls 3 lt -1 t '$n = 4096\times 4096$', \
'data/2048_cannon_dgemm_1.dat' u ($5*1.5):(mean_seq_2048/$1) w boxes ls 4 lt -1 t '$n = 2048\times 2048$'

unset ylabel
set key on
set title "2 wątki"
plot 'data/8192_cannon_dgemm_2.dat' u 5:(mean_seq_8192/$1):xtic(5) w boxes ls 2 lt -1 t '$n = 8192\times 8192$', \
'data/4096_cannon_dgemm_2.dat' u ($5*1.25):(mean_seq_4096/$1) w boxes ls 3 lt -1 t '$n = 4096\times 4096$', \
'data/2048_cannon_dgemm_2.dat' u ($5*1.5):(mean_seq_2048/$1) w boxes ls 4 lt -1 t '$n = 2048\times 2048$'

set ylabel "Przyspieszenie" offset 1
set xlabel "Liczba procesów"
set key off
set title "4 wątki"
plot 'data/8192_cannon_dgemm_4.dat' u 5:(mean_seq_8192/$1):xtic(5) w boxes ls 2 lt -1 t '$n = 8192\times 8192$', \
'data/4096_cannon_dgemm_4.dat' u ($5*1.25):(mean_seq_4096/$1) w boxes ls 3 lt -1 t '$n = 4096\times 4096$', \
'data/2048_cannon_dgemm_4.dat' u ($5*1.5):(mean_seq_2048/$1) w boxes ls 4 lt -1 t '$n = 2048\times 2048$'

unset ylabel
set title "12 wątków"
plot 'data/8192_cannon_dgemm_12.dat' u 5:(mean_seq_8192/$1):xtic(5) w boxes ls 2 lt -1 t '$n = 8192\times 8192$', \
'data/4096_cannon_dgemm_12.dat' u ($5*1.25):(mean_seq_4096/$1) w boxes ls 3 lt -1 t '$n = 4096\times 4096$', \
'data/2048_cannon_dgemm_12.dat' u ($5*1.5):(mean_seq_2048/$1) w boxes ls 4 lt -1 t '$n = 2048\times 2048$'
unset multiplot

set output
set terminal dumb 
replot