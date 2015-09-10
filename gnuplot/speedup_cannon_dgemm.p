#!/usr/bin/gnuplot

reset

#set terminal wxt size 800,600 dashed enhanced font 'Verdana,10' persist

set terminal epslatex size 15cm,9cm  color colortext header \
"\\newcommand{\\ft}[0]{\\footnotesize}"

set output "cannon_dgemm.tex"

set grid xtics ytics ls 3 lw 1 lc rgb 'gray'

set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror out scale 0.75

set xlabel "Liczba procesów"
set ylabel "Przyspieszenie" offset 1,-2.5
set style fill solid 1.0 border rgb 'black'

set border linewidth 1.5 
set style line 1 lc rgb "#355C7D" pt 6 ps 2 lt 1 lw 4 
set style line 2 lc rgb "#6C5B7B" pt 2 ps 2 lt 2 lw 4
set style line 3 lc rgb "#C06C84" pt 3 ps 2 lt 3 lw 4
set style line 4 lc rgb "#F67280" pt 4 ps 2 lt 4 lw 4
set style line 5 lc rgb "#F8B195" pt 5 ps 2 lt 5 lw 4

set style line 8 lt 8 lw 6 lc rgb 'black'
set style line 9 lt 1 lw 1 lc rgb 'white'

set yrange [0:45]
#set ytics 1,1
set ytics auto
set mytics 2
f(x) = mean_f
g(x) = mean_g
h(x) = mean_h

set key right top


fit f(x) 'data/8192_sequential.dat' u 5:1 via mean_f
fit g(x) 'data/4096_sequential.dat' u 5:1 via mean_g
fit h(x) 'data/2048_sequential.dat' u 5:1 via mean_h
set logscale x 2
set multiplot layout 1,3 title "Zestawienie algorytmów"

set key on
set key outside
set border back
set key at 350,46 box ls 9 opaque 

set title "$n=(2048\\times 2048)$"
plot 'data/2048_cannon_dgemm_1.dat' u 5:(mean_h/$1):xticlabel(5) w linesp t 'Cannon-DGEMM, 1 wątek' ls 1, \
'data/2048_cannon_dgemm_2.dat' u 5:(mean_h/$1) w linesp t 'Cannon-DGEMM, 2 wątki' ls 8, \
'data/2048_cannon_dgemm_4.dat' u 5:(mean_h/$1) w linesp t 'Cannon-DGEMM, 4 wątki' ls 2, \
'data/2048_cannon_dgemm_12.dat' u 5:(mean_h/$1) w linesp t 'Cannon-DGEMM, 12 wątków' ls 3, \
'data/2048_cannon_seq.dat' u 5:(mean_h/$1) w linesp t 'Cannon' ls 5, \
1 ls 8 t 'Naiwny'

set key off
unset ylabel
set title "$n=(4096\\times 4096)$"
plot 'data/4096_cannon_dgemm_1.dat' u 5:(mean_g/$1):xticlabel(5) w linesp t 'Cannon-DGEMM, 1 wątek' ls 1, \
'data/4096_cannon_dgemm_2.dat' u 5:(mean_g/$1) w linesp t 'Cannon-DGEMM, 2 wątki' ls 8, \
'data/4096_cannon_dgemm_4.dat' u 5:(mean_g/$1) w linesp t 'Cannon-DGEMM, 4 wątki' ls 2, \
'data/4096_cannon_dgemm_12.dat' u 5:(mean_g/$1) w linesp t 'Cannon-DGEMM, 12 wątków' ls 3, \
'data/4096_cannon_seq.dat' u 5:(mean_g/$1) w linesp t 'Cannon' ls 5, \
1 ls 8 t 'Naiwny'

unset ylabel
set key off
set title "$n=(8192\\times 8192)$"
plot 'data/8192_cannon_dgemm_1.dat' u 5:(mean_f/$1):xticlabel(5) w linesp t 'Cannon-DGEMM, 1 wątek' ls 1, \
'data/8192_cannon_dgemm_2.dat' u 5:(mean_f/$1) w linesp t 'Cannon-DGEMM, 2 wątki' ls 8, \
'data/8192_cannon_dgemm_4.dat' u 5:(mean_f/$1) w linesp t 'Cannon-DGEMM, 4 wątki' ls 2, \
'data/8192_cannon_dgemm_12.dat' u 5:(mean_f/$1) w linesp t 'Cannon-DGEMM, 12 wątków' ls 3, \
'data/8192_cannon_seq.dat' u 5:(mean_f/$1) w linesp t 'Cannon' ls 5, \
1 ls 8 t 'Naiwny'
unset multiplot
set output
set terminal dumb 
replot
