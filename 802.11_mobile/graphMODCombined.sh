#!/bin/bash
#dir="/home/ishtiaq/ns/ns2_data/iTCP/"
dir="../../../out/"
output_file_format="tcp";
under="_"
graph="GRAPH"
all="all"
mod="../../../MOD/out/"

cd out/

mkdir graphs
cd graphs/

mkdir COMBINED
cd COMBINED/

variation=1

while [ $variation -le 4 ] 
do
	if [ $variation -eq 1 ]; then 
		echo 
		echo 
		echo "*********GENERATING GRAPHS VARYING NUMBER OF NODES*********"
		vary="NodeNum"

	elif [ $variation -eq 2 ]; then 
		echo 
		echo 
		echo "*********GENERATING GRAPHS VARYING NUMBER OF FLOWS*********"
		vary="FlowNum"

	elif [ $variation -eq 3 ]; then 
		echo 
		echo 
		echo "*********GENERATING GRAPHS VARYING PACKETS PER SECOND*********"
		vary="PacketsPerSec"

	elif [ $variation -eq 4 ]; then 
		echo 
		echo 
		echo "*********GENERATING GRAPHS VARYING SPEED OF NODES*********"
		vary="NodeSpeed"
			
	else 
		echo 
		echo 
		echo "*********WRONG*********"
	fi

	output_file2="$dir$output_file_format$under$vary$under$graph.out"
	output_file3="$mod$output_file_format$under$vary$under$graph.out"


	gnuplot -persist <<-EOFMarker

		set xlabel '$vary'    

		set ylabel 'THR'
		set term png
		set output "THR VS $vary.png"    
		plot '$output_file2' using 1:2 with linespoints title "Original", \
			 '$output_file3' using 1:2 with linespoints title "Modified"
		set ylabel 'Delay'
		set term png
		set output "Delay VS $vary.png"        
		plot '$output_file2' using 1:3 with linespoints title "Original", \
			 '$output_file3' using 1:3 with linespoints title "Modified"

		set ylabel 'DelRatio'
		set term png
		set output "DelRatio VS $vary.png"        
		plot '$output_file2' using 1:4 with linespoints title "Original", \
			 '$output_file3' using 1:4 with linespoints title "Modified"

		set ylabel 'DropRatio'
		set term png
		set output "DropRatio VS $vary.png"        
		plot '$output_file2' using 1:5 with linespoints title "Original", \
			 '$output_file3' using 1:5 with linespoints title "Modified"

		set ylabel 'TotalEnergy'
		set term png
		set output "TotalEnergy VS $vary.png"        
		plot '$output_file2' using 1:6 with linespoints title "Original", \
			 '$output_file3' using 1:6 with linespoints title "Modified"

	EOFMarker


	echo Done generating $vary variation graphs
	variation=$(($variation+1))


done

cd ../..
