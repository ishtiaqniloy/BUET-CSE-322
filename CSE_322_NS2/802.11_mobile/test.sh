
#dir="/home/ishtiaq/ns/ns2_data/iTCP/"
dir="../../out/"
output_file_format="tcp";
under="_"
graph="GRAPH"
all="all"
nodeThr="nodeThr"
temp="temp"


cd out/
rm -rf graphs

mkdir graphs
cd graphs/


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

	node_thr_file="$dir$output_file_format$under$vary$under$nodeThr.out"


	gnuplot -persist <<-EOFMarker
		set style line 1 \
		    linecolor rgb '#0060ad' \
		    linetype 1 linewidth 2 \
		    pointtype 7 pointsize 1.5

		set xlabel '$vary'    

		set ylabel 'THR'
		set term png
		set output "THR VS $vary.png"    
		plot '$output_file2' using 1:2 with linespoints linestyle 1 title "Thr VS $vary"

		set ylabel 'Delay'
		set term png
		set output "Delay VS $vary.png"    
		plot '$output_file2' using 1:3 with linespoints linestyle 1 title "Delay VS $vary"

		set ylabel 'DelRatio'
		set term png
		set output "DelRatio VS $vary.png"    
		plot '$output_file2' using 1:4 with linespoints linestyle 1 title "DelRatio VS $vary"

		set ylabel 'DropRatio'
		set term png
		set output "DropRatio VS $vary.png"    
		plot '$output_file2' using 1:5 with linespoints linestyle 1 title "DropRatio VS $vary"

		set ylabel 'TotalEnergy'
		set term png
		set output "TotalEnergy VS $vary.png"    
		plot '$output_file2' using 1:6 with linespoints linestyle 1 title "TotalEnergy VS $vary"

	EOFMarker


	num_points_file="$dir$output_file_format$under$vary$under$temp.out"
	rm -f $num_points_file
	touch $num_points_file

	cat $node_thr_file | while read line 
	do

		first_val=$(echo $line | cut -d' ' -f 1)

		if [ $first_val -eq -1 ]; then
			curr_num_point=$(echo $line | cut -d' ' -f $(($variation+2)))
			#echo $curr_num_point
			echo $curr_num_point >> $num_points_file
			temp_file="$dir$output_file_format$under$vary$under$temp$curr_num_point.out"
			rm -f $temp_file
			touch $temp_file

			#echo $temp_file

		else
			echo $line >> $temp_file

		fi
	
	done


	gnuplot -persist <<-EOFMarker
		set style line 1 \
		    linecolor rgb '#0060ad' \
		    linetype 1 linewidth 2 \
		    pointtype 7 pointsize 1.5

		set xlabel '$vary'    

		set ylabel 'THR'
		set term png
		set output "THR VS $vary.png"    
		plot '$output_file2' using 1:2 with linespoints linestyle 1 title "Thr VS $vary"

		set ylabel 'Delay'
		set term png
		set output "Delay VS $vary.png"    
		plot '$output_file2' using 1:3 with linespoints linestyle 1 title "Delay VS $vary"

		set ylabel 'DelRatio'
		set term png
		set output "DelRatio VS $vary.png"    
		plot '$output_file2' using 1:4 with linespoints linestyle 1 title "DelRatio VS $vary"

		set ylabel 'DropRatio'
		set term png
		set output "DropRatio VS $vary.png"    
		plot '$output_file2' using 1:5 with linespoints linestyle 1 title "DropRatio VS $vary"

		set ylabel 'TotalEnergy'
		set term png
		set output "TotalEnergy VS $vary.png"    
		plot '$output_file2' using 1:6 with linespoints linestyle 1 title "TotalEnergy VS $vary"

	EOFMarker

	########GRAPH WILL BE GENERATED IN BANGLA METHOD#######################


	echo Done generating $vary variation graphs
	variation=$(($variation+1))


done

cd ../..
