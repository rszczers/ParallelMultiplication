#!/usr/bin/gnuplot

reset

#set terminal wxt size 800,600 dashed enhanced font 'Verdana,10' persist

set terminal epslatex size 12cm,8cm color colortext header \
"\\newcommand{\\ft}[0]{\\footnotesize}"

set output "cannon_diff_n.tex"

set grid xtics ytics ls 3 lw 1 lc rgb 'gray'

set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror out scale 0.75

set xlabel "Liczba procesów"
set ylabel "Przyspieszenie"
set style fill solid 1.0 border rgb 'black'

set border linewidth 1.5 

set style line 1 lc rgb "#355C7D" pt 6 ps 2 lt 1 lw 1 
set style line 2 lc rgb "#6C5B7B" pt 2 ps 2 lt 1 lw 1
set style line 3 lc rgb "#C06C84" pt 3 ps 2 lt 1 lw 1
set style line 4 lc rgb "#F67280" pt 4 ps 2 lt 1 lw 1
set style line 5 lc rgb "#F8B195" pt 5 ps 2 lt 1 lw 1

set style line 8 lt 8 lw 6 lc rgb 'black'

set yrange [0:9]
#set ytics 1,1
set ytics auto
set mytics 2

set boxwidth 0.25
set style fill solid
set logscale x
f(x) = mean_seq_2048
g(x) = mean_seq_4096
h(x) = mean_seq_8192
set title "Klasyczny algorytm Cannona"
set key left top

fit f(x) 'data/2048_sequential.dat' u 5:1 via mean_seq_2048
fit g(x) 'data/4096_sequential.dat' u 5:1 via mean_seq_4096
fit h(x) 'data/8192_sequential.dat' u 5:1 via mean_seq_8192
set samples 8
set style fill pattern border

plot 'data/2048_cannon_seq.dat' u 5:(mean_seq_2048/$1):xtic(5) w boxes ls 1 lt -1 t '$n = 2048\times 2048$', \
'data/4096_cannon_seq.dat' u ($5*1.25):(mean_seq_4096/$1) w boxes ls 2 lt -1 t '$n = 4096\times 4096$', \
'data/8192_cannon_seq.dat' u ($5*1.5):(mean_seq_8192/$1) w boxes ls 3 lt -1 t '$n = 8192\times 8192$'

set output
set terminal dumb 
replot
