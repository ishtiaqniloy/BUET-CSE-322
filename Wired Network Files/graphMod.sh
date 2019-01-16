#!/bin/bash
#dir="/home/ishtiaq/ns/ns2_data/iTCP/"
dir="../../../out/"
output_file_format="tcp";
under="_"
graph="GRAPH"
all="all"
nodeThr="nodeThr"
temp="temp"
mod="../../../MOD/"

cd out/

mkdir Combinedgraphs
cd Combinedgraphs/

mkdir COMBINED
cd COMBINED/

variation=1

while [ $variation -le 3 ] 
do
	if [ $variation -eq 1 ]; then 
		echo 
		echo 
		echo "*********GENERATING GRAPHS VARYING NUMBER OF NODES*********"
		vary="tcp_node_var_avg"
		vary2="tcp_1_conges_avg_"

		name="NodeNum"

	elif [ $variation -eq 2 ]; then 
		echo 
		echo 
		echo "*********GENERATING GRAPHS VARYING NUMBER OF FLOWS*********"
		vary="tcp_flow_var_avg"
		vary2="tcp_2_conges_avg_"
		name="flowNum"


	elif [ $variation -eq 3 ]; then 
		echo 
		echo 
		echo "*********GENERATING GRAPHS VARYING PACKETS PER SECOND*********"
		vary="tcp_pkt_rate_var_avg"
		vary2="tcp_3_conges_avg_"
		name="pktRate"


			
	else 
		echo 
		echo 
		echo "*********WRONG*********"
	fi

	output_file2="$dir$vary.dat"
	output_file3="$mod$vary.dat"

	gnuplot -persist <<-EOFMarker

		set xlabel '$name'    

		set ylabel 'THR'
		set term png
		set output "THR VS $name.png"    
		plot '$output_file2' using 1:2 with linespoints title "Original", \
			 '$output_file3' using 1:2 with linespoints title "Modified"
		set ylabel 'Delay'
		set term png
		set output "Delay VS $name.png"        
		plot '$output_file2' using 1:3 with linespoints title "Original", \
			 '$output_file3' using 1:3 with linespoints title "Modified"

		set ylabel 'DelRatio'
		set term png
		set output "DelRatio VS $name.png"        
		plot '$output_file2' using 1:4 with linespoints title "Original", \
			 '$output_file3' using 1:4 with linespoints title "Modified"

		set ylabel 'DropRatio'
		set term png
		set output "DropRatio VS $name.png"        
		plot '$output_file2' using 1:5 with linespoints title "Original", \
			 '$output_file3' using 1:5 with linespoints title "Modified"

	EOFMarker


	echo Done generating $vary variation graphs
	variation=$(($variation+1))


done

cd ../..
