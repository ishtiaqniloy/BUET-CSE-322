#!/usr/bin/gnuplot -persist

set title "throughput graph in node variation"
set xlabel "nodes"
set ylabel "throughput"
set grid

set offset 1,1,1,1
plot "tcp_node_var_avg.dat" u 1:2:(sprintf("(%d, %d)", $1, $2)) with labels point  pt 7 offset char 1,1 title "throughput"
