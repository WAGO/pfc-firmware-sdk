#!/usr/bin/gnuplot

reset

#svg 
set terminal svg size 2000,1000 fname 'Verdana, Helvetica, Arial, sans-serif' fsize '9' rounded dashed

set output 'cpu_load.svg'

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
# define grid
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12

set xlabel 'time [2s]'
set ylabel 'cpu [%]'
set xrange [0:200]
set yrange [0:100]

set datafile separator ","

plot 'cpu_load.csv' u 0:1 t 'total' w l,  	\
     '' u 0:2 t 'rts' w l,					\
     '' u 0:3 t 'rts/def' w l,				\
     '' u 0:4 t 'rts/iec' w l,				\
     '' u 0:5 t 'profinet' w l,				\
     '' u 0:6 t 'profinet/fifo' w l,		\
     '' u 0:7 t 'can' w l, 					\
     '' u 0:8 t 'kbus'