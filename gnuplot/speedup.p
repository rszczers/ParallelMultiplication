reset

set terminal wxt size 800,600 dashed enhanced font 'Verdana,10' persist
#set terminal epslatex size 3.5,2.62 color colortext
#set terminal epslatex size 3.5,2.62 standalone color colortext 10
#set output "plot.tex"

set grid xtics ytics ls 3 lw 1 lc rgb 'gray'

set xlabel 'Liczba procesów'
set ylabel 'Przyspieszenie względem algorytmu cblas_dgemm z pakietu MKL'
set style fill solid 1.0 border rgb 'black'

#set border linewidth 1.5
set style line 1 lc rgb 'black' pt 7 ps 1 lt 1 lw 2 

set style line 2 lt 7 lw 2


set style line 11 lc rgb '#808080' lt 1
set border 3 front ls 11
set tics nomirror out scale 1 

#set logscale x 2
set nologscale y


unset key
f(x) = mean_seq
fit f(x) 'mkl.dat' u 5:1 via mean_seq

plot 'cannon.dat' u (mean_seq/$1):xticlabel(5) w linesp t 'Cannon' ls 1, 1 ls 2 
set output
set terminal X11
replot
