#INPUT: output file AND number of iterations
output_file_format="tcp";
iteration_float=10.0;


#dir="/home/ishtiaq/ns/ns2_data/iTCP/"
dir="out/"
under="_"
all="all"
nodeThr="nodeThr"

node_num=100	#max val

rm -rf out
mkdir out

iteration=$(printf %.0f $iteration_float);


	#deleting previous files
	output_file="$dir$output_file_format.out"
	rm -f $output_file
	touch $output_file

	node_thr_file="$dir$output_file_format$under$nodeThr.out"
	rm -f $node_thr_file
	touch $node_thr_file

	echo "total iteration: $iteration"
	l=0;thr=0.0;del=0.0;s_packet=0.0;r_packet=0.0;d_packet=0.0;del_ratio=0.0;
	dr_ratio=0.0;time=0.0;t_energy=0.0;energy_bit=0.0;energy_byte=0.0;energy_packet=0.0;total_retransmit=0.0;


		output_file="$dir$output_file_format$under$all.out"
		rm -f $output_file
		touch $output_file


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

	ns wired-cum-wireless-sim.tcl
	echo "SIMULATION COMPLETE. BUILDING STAT......"
	#awk -f rule_th_del_enr_tcp.awk 802_11_grid_tcp_with_energy_random_traffic.tr > math_model1.out
	awk -f wired_cum_wireless.awk out/wired-and-wireless.tr > out/wired_cum_wireless.out

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
			total_retransmit=$(echo "scale=5; $total_retransmit+$val/$iteration_float" | bc)
			echo -ne "total_retrnsmit: $val \n" >> $output_file
		else 
			#echo line: $val
			n_num=$(echo $val | cut -d' ' -f 1)
			n_thr=$(echo $val | cut -d' ' -f 2)

			node_thr[$n_num]=$(echo "scale=5; ${node_thr[$n_num]}+$n_thr/$iteration_float" | bc)

			if [ $n_num -lt $node_num ]; then
				echo node throughput : $n_num $n_thr
			fi

		fi

	if [ $l -le 9 ]; then
		echo "$val"
	fi

	done < out/wired_cum_wireless.out

	i=$(($i+1))
	l=0


	echo "******END OF ITERATION: $i******"

	#======================================
	#END AN ITERATION
	#======================================
	done

	echo "******END OF ITERATIONS******"

	echo ""
	echo "******PRINTING AVERAGE STAT******"

	output_file="$dir$output_file_format.out"

	echo -ne "Throughput:          $thr " >> $output_file
	echo -ne "AverageDelay:         $del " >> $output_file
	echo -ne "Sent Packets:         $s_packet " >> $output_file
	echo -ne "Received Packets:         $r_packet " >> $output_file
	echo -ne "Dropped Packets:         $d_packet " >> $output_file

	echo -ne "PacketDeliveryRatio:      $del_ratio " >> $output_file
	echo -ne "PacketDropRatio:      $dr_ratio " >> $output_file
	echo -ne "Total time:  $time " >> $output_file
	echo "total_retransmit:         $total_retransmit " >> $output_file

	echo "Throughput:          $thr " 
	echo "AverageDelay:         $del "
	echo "Sent Packets:         $s_packet "
	echo "Received Packets:         $r_packet "
	echo "Dropped Packets:         $d_packet "

	echo "PacketDeliveryRatio:      $del_ratio "
	echo "PacketDropRatio:      $dr_ratio "
	echo "Total time:  $time "
	echo "total_retransmit:         $total_retransmit "

	j=0
	echo -1 >> $node_thr_file
	while [ $j -lt $node_num ]; do


		temp_thr=$(printf %.0f ${node_thr[$j]});

		if [ $temp_thr -gt 0 ]; then
			echo $j ${node_thr[$j]} >> $node_thr_file
			echo Per-node Throughput: $j ${node_thr[$j]}

		fi

		j=$(($j+1))

	done

	echo ""




echo "******END OF SIMULATION******"



echo exiting...
