#!/bin/bash
#INPUT: output file AND number of iterations
output_file_format="tcp";
iteration_float=2.0;

start=5
end=5

node=20
flow=10
npkt=100
node_arr=( 20 40 60 80 100 )
#node_arr=( 20 )
flow_arr=( 10 20 30 40 50 )
pkt_arr=( 100 200 300 400 500 )

variation=3
var=1

iteration=$(printf %.0f $iteration_float);

r=$start
dir="/home/shashata/Documents/NS2/ns temp/TemporaryRepo/wired_tries/"
folder="out/"
mkdir out

while [ $var -le $variation ]
do

	if [ $var -eq 1 ]; then
		echo "Varying nodes"
		flow=30
		npkt=10
		exec_arr=("${node_arr[@]}")
	elif [ $var -eq 2 ]; then
		echo "Varying flow"
		node=40
		npkt=10
		exec_arr=("${flow_arr[@]}")
	elif [ $var -eq 3 ]; then
		echo "Varying packet rate"
		node=40
		flow=30
		exec_arr=("${pkt_arr[@]}")
	fi


	for k in ${exec_arr[*]}
	do
		if [ $var -eq 1 ]; then
			node=$k
			echo "========== Executing $k node variation =========="
		elif [ $var -eq 2 ]; then 
			flow=$k
			echo "========== Executing $k flow variation =========="
		elif [ $var -eq 3 ]; then
			npkt=$k
			echo "========== Executing $k packet_rate variation =========="
		else
			echo "error in $var value "
		fi
		echo "total iteration: $iteration"
		###############################START A ROUND
		l=0;thr=0.0;del=0.0;s_packet=0.0;r_packet=0.0;d_packet=0.0;del_ratio=0.0;
		dr_ratio=0.0;time=0.0;t_energy=0.0;energy_bit=0.0;energy_byte=0.0;energy_packet=0.0;total_retransmit=0.0;

		i=0
		flag2=0
			while [ $i -lt $iteration ]
			do
			#################START AN ITERATION
			echo "                             EXECUTING $(($i+1)) th ITERATION"

			ns-v1 wired_tcl_for_tcp.tcl $node $flow $npkt
			echo "SIMULATION COMPLETE. BUILDING STAT......"
			#awk -f rule_th_del_enr_tcp.awk 802_11_grid_tcp_with_energy_random_traffic.tr > math_model1.out
			awk -f tcp_wired_sam.awk tcp_wired.tr > out/tcp_wired.out

			while read val
			do
			#	l=$(($l+$inc))
				l=$(($l+1))

				
				#dir=""
				under="_"
				all="all"
				output_file="$folder$output_file_format$under$var$under$var$under$all.out"
				
			#	echo -ne "Throughput:          $thr " > "$output_file"
				if [ "$l" == "1" ]; then
					thr=$(echo "scale=5; $thr+$val/$iteration_float" | bc)
					echo -ne "throughput: $val " >> "$output_file"
				elif [ "$l" == "2" ]; then
					del=$(echo "scale=5; $del+$val/$iteration_float" | bc)
					echo -ne "delay: $val " >> "$output_file"
				elif [ "$l" == "3" ]; then
					s_packet=$(echo "scale=5; $s_packet+$val/$iteration_float" | bc)
					echo -ne "send packet: $val " >> "$output_file"
				elif [ "$l" == "4" ]; then
					r_packet=$(echo "scale=5; $r_packet+$val/$iteration_float" | bc)
					echo -ne "received packet: $val " >> "$output_file"
				elif [ "$l" == "5" ]; then
					d_packet=$(echo "scale=5; $d_packet+$val/$iteration_float" | bc)
					echo -ne "drop packet: $val " >> "$output_file"
				elif [ "$l" == "6" ]; then
					del_ratio=$(echo "scale=5; $del_ratio+$val/$iteration_float" | bc)
					echo -ne "delivery ratio: $val " >> "$output_file"
				elif [ "$l" == "7" ]; then
					dr_ratio=$(echo "scale=5; $dr_ratio+$val/$iteration_float" | bc)
					echo -ne "drop ratio: $val " >> "$output_file"
				elif [ "$l" == "8" ]; then
					time=$(echo "scale=5; $time+$val/$iteration_float" | bc)
					echo -ne "time: $val \n" >> "$output_file"

				fi
				


				echo "$val"
			done < out/tcp_wired.out
#incorporate congestion calculation here

			var2="Congestion_value"
			var3="conges_avg"
			output_file="$folder$output_file_format$under$var$under$var2$under$all.out"
			output_file2="$folder$output_file_format$under$var$under$var3$under.out"
			prev_val=0.0
			flag=0
			sum=0
			count=1
			conges_avg=0.0
			declare -A hashtbl
			
			if [ $var -eq 1 -a $flag2 -eq 0 ]; then
				echo "========= $node node variation congestion all output ==========" >> "$output_file"
				echo "-2 $var $node $flow $npkt" >> "$output_file2"
				flag2=1
			elif [ $var -eq 2 -a $flag2 -eq 0 ]; then
				echo "========= $flow flow variation congestion all output ==========" >> "$output_file"
				echo "-2 $var $node $flow $npkt" >> "$output_file2"
				flag2=1
			elif  [ $var -eq 3 -a $flag2 -eq 0 ]; then
				echo "========= $npkt packet_rate variation congestion all output ==========" >> "$output_file"
				echo "-2 $var $node $flow $npkt" >> "$output_file2"
				flag2=1
			fi
			
			echo "============= $i iteration data ============" >> "$output_file"
			while IFS=: read -r line
			do
				#echo $line
				sl=0
				for word in $line
				do 
					sl=$(($sl+1))
					if [ "$sl" == "1" ]; then
						time=$word
						if [ "$word" != "$prev_val" ]; then
							#echo "$word"
							
							flag=1
							
							#avg_val=`echo $sum/$count | bc`
							#VAR=$(bc <<<"scale=2;$IMG_WIDTH/$IMG2_WIDTH")
							count=`echo $count + 0.0 | bc`
							#prev_val=$(echo "scale=2; $prev_val+0.0" | bc -l)
							prev_val=$(echo "scale=2; $prev_val/1.0" | bc)
							#echo "The count is $count, the sum is $sum" 
							avg_node_conges=$(echo "scale=5; $sum/$count" | bc)
							echo "Time = $prev_val congestion ratio: $avg_node_conges " >> "$output_file"
							if [ $i -eq 0 ]; then
								avg_itr_val=$(echo "scale=5; $avg_node_conges/$iteration_float" | bc)
								hashtbl["$prev_val"]="$avg_itr_val"
							else
								temp_val=${hashtbl["$prev_val"]}
								avg_itr_val=$(echo "scale=5; $temp_val+$avg_node_conges/$iteration_float" | bc)
								hashtbl["$prev_val"]="$avg_itr_val"
								if (( i == iteration-1 )); then
									time=$(echo "scale=2; $time/1.0" | bc)
									#echo -ne "nice"
									echo  "$i $prev_val ${hashtbl["$prev_val"]}"
									echo "$prev_val ${hashtbl["$prev_val"]}" >> "$output_file2"
								fi
								
							fi
								#statementselif
							#echo "time = $prev_val, avg_val = $avg_val"
							
							sum=0
							count=1
							prev_val=$word
						else
							flag=0
							
							
						fi
					elif [ "$sl" == "2" ]; then
						if [ $flag -eq 0 ];then 
							#echo $word
							count=$(($count+1))
						fi
					else
						#echo $word
						sum=`echo $sum + $word | bc`
						#sum=$(( $sum + $word )) 
						

					fi

				done	
			done < out/congestionData.dat
			#echo "count= $count flow = $flow"
			if [ "$count" == "$flow" ]; then
				#echo "Last values processing $time $count $sum" 
				time=$(echo "scale=2; $time/1.0" | bc)
				avg_val=$(echo "scale=5; $sum/$count" | bc)
				echo "Time = $time congestion ratio: $avg_node_conges " >> "$output_file"
				if [ $i -eq 0 ]; then
					avg_itr_val=$(echo "scale=5; $avg_node_conges/$iteration_float" | bc)
					hashtbl["$time"]="$avg_itr_val"
				else
					temp_val=${hashtbl["$time"]}
					avg_itr_val=$(echo "scale=5; $temp_val+$avg_node_conges/$iteration_float" | bc)
					hashtbl["$time"]="$avg_itr_val"
					if (( i == iteration-1 )); then
						time=$(echo "scale=2; $time/1.0" | bc)
						#echo -ne "nice"
						echo  "$i $time ${hashtbl["$time"]}"
						echo "$time ${hashtbl["$time"]}" >> "$output_file2"
					fi
				fi
			fi
			#conges_avg=$(echo "scale=5; $conges_avg+$avg_node_conges/$iteration_float" | bc)
			
				
				
			
			i=$(($i+1))
			l=0
			#################END AN ITERATION
		done


	under="_"
	
	if [ $var -eq 1 ]; then
		echo "========== $node node variation executed =========="
		fname="node_var_avg"
		output_file="$folder$output_file_format$under$fname.out"
		echo "========== $node node variation avg result ==========" >> "$output_file"

		echo "flusing $node node info"
		data_file="$folder$output_file_format$under$fname.dat"
		echo "$node $thr $del $del_ratio $dr_ratio" >> "$data_file"

	elif [ $var -eq 2 ]; then 	
		echo "========== $ flow variation executed =========="
		fname="flow_var_avg"
		output_file="$folder$output_file_format$under$fname.out"
		echo "========== $k flow variation avg result ==========" >> "$output_file"

		echo "flusing $flow flow info"
		data_file="$folder$output_file_format$under$fname.dat"
		echo "$flow $thr $del $del_ratio $dr_ratio" >> "$data_file"
	elif [ $var -eq 3 ]; then 
		echo "========== $npkt packet_rate variation executed =========="
		fname="pkt_rate_var_avg"
		output_file="$folder$output_file_format$under$fname.out"
		echo "========== $npkt packet_rate variation avg result ==========" >> "$output_file"

		echo "flusing $npkt packet_rate info"
		data_file="$folder$output_file_format$under$fname.dat"
		echo "$npkt $thr $del $del_ratio $dr_ratio" >> "$data_file"
	else
		echo "error in $var value "
	fi 

	#dir=""
	
	

	echo -ne "Throughput:          $thr " >> "$output_file"
	echo -ne "AverageDelay:         $del " >> "$output_file"
	echo -ne "Sent Packets:         $s_packet " >> "$output_file"
	echo -ne "Received Packets:         $r_packet " >> "$output_file"
	echo -ne "Dropped Packets:         $d_packet " >> "$output_file"
	echo -ne "PacketDeliveryRatio:      $del_ratio " >> "$output_file"
	echo -ne "PacketDropRatio:      $dr_ratio " >> "$output_file"
	echo -ne "Total time:  $time \n" >> "$output_file"

	

	#######################################END A SPECIFIC VARIATION
done
var=$(($var+1))
#######################################END OF A VARIATION Type
done
