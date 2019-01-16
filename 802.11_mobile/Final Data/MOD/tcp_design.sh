#!/bin/bash
#ns parameters format:
#ns <filename.tcl> #row #col #flows #packets_per_sec #node_speed

#======================================
#default values
#======================================
def_num_row=6
def_num_col=10
def_num_flows=30
def_packets_per_sec=300
def_node_speed=15



#======================================
#maximum values###
#======================================
max_node_num=100
max_num_flows=50
max_packets_per_sec=500
max_node_speed=25

#1->nodes 2->flow 3->packet 4->speed
variation=1

#INPUT: output file AND number of iterations
output_file_format="tcp";
iteration_float=10.0;


#dir="/home/ishtiaq/ns/ns2_data/iTCP/"
dir="out/"
under="_"
graph="GRAPH"
all="all"
nodeThr="nodeThr"

rm -rf out
mkdir out

iteration=$(printf %.0f $iteration_float);

while [ $variation -le 4 ] 
do
	flag=1
			
	if [ $variation -eq 1 ]; then 
		echo 
		echo 
		echo "*********VARYING NUMBER OF NODES*********"
		vary="NodeNum"
		#initial values
		num_row=4
		num_col=5
		num_flows=$def_num_flows
		packets_per_sec=$def_packets_per_sec
		node_speed=$def_node_speed

	elif [ $variation -eq 2 ]; then 
		echo 
		echo 
		echo "*********VARYING NUMBER OF FLOWS*********"
		vary="FlowNum"
		#initial values
		num_row=$def_num_row
		num_col=$def_num_col
		num_flows=10
		packets_per_sec=$def_packets_per_sec
		node_speed=$def_node_speed

	elif [ $variation -eq 3 ]; then 
		echo 
		echo 
		echo "*********VARYING PACKETS PER SECOND*********"
		vary="PacketsPerSec"
		#initial values
		num_row=$def_num_row
		num_col=$def_num_col
		num_flows=$def_num_flows
		packets_per_sec=100
		node_speed=$def_node_speed

	elif [ $variation -eq 4 ]; then 
		echo 
		echo 
		echo "*********VARYING SPEED OF NODES*********"
		vary="NodeSpeed"
		#initial values
		num_row=$def_num_row
		num_col=$def_num_col
		num_flows=$def_num_flows
		packets_per_sec=$def_packets_per_sec
		node_speed=5
			
	else 
		echo 
		echo 
		echo "*********WRONG*********"
	fi

	#deleting previous files
	output_file="$dir$output_file_format$under$vary.out"
	rm -f $output_file
	touch $output_file

	output_file2="$dir$output_file_format$under$vary$under$graph.out"
	rm -f $output_file2
	touch $output_file2

	node_thr_file="$dir$output_file_format$under$vary$under$nodeThr.out"
	rm -f $node_thr_file
	touch $node_thr_file

	while [ $flag -eq 1 ]
	do
	#======================================
	#START A ROUND
	#======================================
	echo "total iteration: $iteration"
	l=0;thr=0.0;del=0.0;s_packet=0.0;r_packet=0.0;d_packet=0.0;del_ratio=0.0;
	dr_ratio=0.0;time=0.0;t_energy=0.0;energy_bit=0.0;energy_byte=0.0;energy_packet=0.0;total_retransmit=0.0;


		output_file="$dir$output_file_format$under$all.out"
		rm -f $output_file
		touch $output_file

	node_num=$(($num_row * $num_col))

	j=0
	node_thr=()
	while [ $j -lt $node_num ]; do
		node_thr+=(0)
		j=$(($j+1))

	done



	i=0
	flag2=0
	while [ $i -lt $iteration ]
	do
	#======================================
	#START AN ITERATION
	#======================================
	echo "                             EXECUTING $(($i+1)) th ITERATION"

	#echo -ne "iteration: $i " >> $output_file

	echo varying $vary : $node_num $num_flows $packets_per_sec $node_speed

	ns tcp_design_for_802_11.tcl $num_row $num_col $num_flows $packets_per_sec $node_speed
	echo "SIMULATION COMPLETE. BUILDING STAT......"
	#awk -f rule_th_del_enr_tcp.awk 802_11_grid_tcp_with_energy_random_traffic.tr > math_model1.out
	awk -f rule_tcp_wireless.awk out/tcp_wireless.tr > out/tcp_wireless.out

	while read val
	do
	#	l=$(($l+$inc))
		l=$(($l+1))


		
	#	echo -ne "Throughput:          $thr " > $output_file

		if [ "$l" == "1" ]; then
			thr=$(echo "scale=5; $thr+$val/$iteration_float" | bc)
			echo -ne "throughput: $val " >> $output_file
		elif [ "$l" == "2" ]; then
			del=$(echo "scale=5; $del+$val/$iteration_float" | bc)
			echo -ne "delay: $val " >> $output_file
		elif [ "$l" == "3" ]; then
			s_packet=$(echo "scale=5; $s_packet+$val/$iteration_float" | bc)
			echo -ne "send packet: $val " >> $output_file
		elif [ "$l" == "4" ]; then
			r_packet=$(echo "scale=5; $r_packet+$val/$iteration_float" | bc)
			echo -ne "received packet: $val " >> $output_file
		elif [ "$l" == "5" ]; then
			d_packet=$(echo "scale=5; $d_packet+$val/$iteration_float" | bc)
			echo -ne "drop packet: $val " >> $output_file
		elif [ "$l" == "6" ]; then
			del_ratio=$(echo "scale=5; $del_ratio+$val/$iteration_float" | bc)
			echo -ne "delivery ratio: $val " >> $output_file
		elif [ "$l" == "7" ]; then
			dr_ratio=$(echo "scale=5; $dr_ratio+$val/$iteration_float" | bc)
			echo -ne "drop ratio: $val " >> $output_file
		elif [ "$l" == "8" ]; then
			time=$(echo "scale=5; $time+$val/$iteration_float" | bc)
			echo -ne "time: $val " >> $output_file
		elif [ "$l" == "9" ]; then
			t_energy=$(echo "scale=5; $t_energy+$val/$iteration_float" | bc)
			echo -ne "total_energy: $val " >> $output_file
		elif [ "$l" == "10" ]; then
			energy_bit=$(echo "scale=5; $energy_bit+$val/$iteration_float" | bc)
			echo -ne "energy_per_bit: $val " >> $output_file
		elif [ "$l" == "11" ]; then
			energy_byte=$(echo "scale=5; $energy_byte+$val/$iteration_float" | bc)
			echo -ne "energy_per_byte: $val " >> $output_file
		elif [ "$l" == "12" ]; then
			energy_packet=$(echo "scale=5; $energy_packet+$val/$iteration_float" | bc)
			echo -ne "energy_per_packet: $val " >> $output_file
		elif [ "$l" == "13" ]; then
			total_retransmit=$(echo "scale=5; $total_retransmit+$val/$iteration_float" | bc)
			echo -ne "total_retrnsmit: $val \n" >> $output_file
		else 
			n_num=$(echo $val | cut -d' ' -f 1)
			n_thr=$(echo $val | cut -d' ' -f 2)

			node_thr[$n_num]=$(echo "scale=5; ${node_thr[$n_num]}+$n_thr/$iteration_float" | bc)

			#if [ $n_num -lt $node_num ]; then
			#	echo node throughput : $n_num $n_thr ${node_thr[$n_num]}
			#fi

		fi

	if [ $l -le 13 ]; then
		echo "$val"
	fi

	done < out/tcp_wireless.out

	#incorporate congestion calculation here

			var2="Congestion_value"
			var3="conges_avg"
			output_file="$dir$output_file_format$under$vary$under$var2$under$all.out"
			output_file2="$dir$output_file_format$under$vary$under$var3$under.out"
			prev_val=0.0
			flag3=0
			sum=0
			count=1
			conges_avg=0.0
			declare -A hashtbl
			
			if [ $variation -eq 1 -a $flag2 -eq 0 ]; then
				echo "========= $node_num node variation congestion all output ==========" >> "$output_file"
				echo "-2 $variation $node_num $num_flows $packets_per_sec $node_speed" >> "$output_file2"
				flag2=1
			elif [ $variation -eq 2 -a $flag2 -eq 0 ]; then
				echo "========= $num_flows flow variation congestion all output ==========" >> "$output_file"
				echo "-2 $variation $node_num $num_flows $packets_per_sec $node_speed" >> "$output_file2"
				flag2=1
			elif  [ $variation -eq 3 -a $flag2 -eq 0 ]; then
				echo "========= $packets_per_sec packet_rate variation congestion all output ==========" >> "$output_file"
				echo "-2 $variation $node_num $num_flows $packets_per_sec $node_speed" >> "$output_file2"
				flag2=1
			elif  [ $variation -eq 4 -a $flag2 -eq 0 ]; then
				echo "========= $node_speed node_speed variation congestion all output ==========" >> "$output_file"
				echo "-2 $variation $node_num $num_flows $packets_per_sec $node_speed" >> "$output_file2"
				flag2=1
			fi
			
			echo "============= $i iteration data ============" >> "$output_file"
			echo LOOP1 BROKEN
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
							
							flag3=1
							
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
								#echo $prev_val ${hashtbl["$prev_val"]} 
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
							flag3=0
							
							
						fi
					elif [ "$sl" == "2" ]; then
						if [ $flag3 -eq 0 ];then 
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
	#======================================
	#END AN ITERATION
	#======================================
	done

	output_file="$dir$output_file_format$under$vary.out"
	output_file2="$dir$output_file_format$under$vary$under$graph.out"


	if [ $variation -eq 1 ]; then 
		param_val=$(($num_row * $num_col))
			echo "*********done node_num $param_val*********"

	elif [ $variation -eq 2 ]; then
		param_val=$num_flows
			echo "*********done num_flows $param_val*********"

	elif [ $variation -eq 3 ]; then
		param_val=$packets_per_sec
			echo "*********done packets_per_sec $param_val*********"

	elif [ $variation -eq 4 ]; then
		param_val=$node_speed
			echo "*********done node_speed $param_val*********"
			
	else
		echo "*********WRONG*********"
	fi

	echo -ne "Parameter:			$param_val " >> $output_file
	echo -ne "$param_val " >> $output_file2

	echo -ne "Throughput:          $thr " >> $output_file
	echo -ne "$thr " >> $output_file2

	echo -ne "AverageDelay:         $del " >> $output_file
	echo -ne "$del " >> $output_file2

	echo -ne "Sent Packets:         $s_packet " >> $output_file
	echo -ne "Received Packets:         $r_packet " >> $output_file
	echo -ne "Dropped Packets:         $d_packet " >> $output_file

	echo -ne "PacketDeliveryRatio:      $del_ratio " >> $output_file
	echo -ne "$del_ratio " >> $output_file2

	echo -ne "PacketDropRatio:      $dr_ratio " >> $output_file
	echo -ne "$dr_ratio " >> $output_file2

	echo -ne "Total time:  $time " >> $output_file

	echo -ne "Total energy consumption:        $t_energy " >> $output_file
	echo  "$t_energy " >> $output_file2

	echo -ne "Average Energy per bit:         $energy_bit " >> $output_file
	echo -ne "Average Energy per byte:         $energy_byte " >> $output_file
	echo -ne "Average energy per packet:         $energy_packet " >> $output_file
	echo "total_retransmit:         $total_retransmit " >> $output_file


	j=0
	echo -1 $variation $node_num $num_flows $packets_per_sec $node_speed >> $node_thr_file
	while [ $j -lt $node_num ]; do


		temp_thr=$(printf %.0f ${node_thr[$j]});

		if [ $temp_thr -gt 0 ]; then
			echo $j ${node_thr[$j]} >> $node_thr_file
			echo Per-node Throughput: $j ${node_thr[$j]}

		fi

		j=$(($j+1))

	done


	if [ $variation -eq 1 ]; then 
		#nodes
		node_num=$(($num_row * $num_col))
		if [ $node_num -eq 20 ]; then 
			#echo "*********ROUND 1 COMPLETE*********"
			num_row=5
			num_col=8

		elif [ $node_num -eq 40 ]; then
			#echo "*********ROUND 2 COMPLETE*********"
			num_row=6
			num_col=10

		elif [ $node_num -eq 60 ]; then
			#echo "*********ROUND 3 COMPLETE*********"
			num_row=8

		elif [ $node_num -eq 80 ]; then
			#echo "*********ROUND 4 COMPLETE*********"
			num_row=10
			
		else
			#echo "*********ROUND 5 COMPLETE*********"
			num_row=100
			num_col=100
			flag=0
		fi

		node_num=$(($num_row * $num_col))

		echo "*********new node_num $node_num*********"

	elif [ $variation -eq 2 ]; then
		#flows
		if [ $num_flows -lt $max_num_flows ]; then
			num_flows=$(($num_flows+10))
			echo "*********new num_flows $num_flows*********"
		else
			flag=0
		fi

	elif [ $variation -eq 3 ]; then
		#packets per sec
		if [ $packets_per_sec -lt $max_packets_per_sec ]; then
			packets_per_sec=$(($packets_per_sec+100))
			echo "*********new packets_per_sec $packets_per_sec*********"
		else
			flag=0
		fi

	elif [ $variation -eq 4 ]; then
		#speed
		if [ $node_speed -lt $max_node_speed ]; then
			node_speed=$(($node_speed+5))
			echo "*********new node_speed $node_speed*********"
		else
			flag=0
		fi
			
	else
		echo "*********WRONG*********"
		flag=0
	fi

			

	#======================================
	#END A ROUND
	#======================================
	done


	#======================================
	#END OF A VARIATION
	#======================================
	echo done varying $vary
	variation=$(($variation+1))

done

echo "******END OF SIMULATION******"


./graph.sh

echo exiting...
