
#INPUT: output file AND number of iterations
iteration_float=1.0;
iteration=$(printf %.0f $iteration_float);

k=0;
endfinal=10;
while [ $k -le  $endfinal ]
do


start=1
end=5
r=$start

whichTerm=""
if [ "$k" == "0" ];then
	whichTerm="nodes"
elif [ "$k" == "5" ];then
	whichTerm="flows"
elif [ "$k" == "10" ];then
	whichTerm="packet"
fi
output_file="tcp_all_wired_for_$whichTerm.out"
output_file1="tcp_wired_for_$whichTerm.out"
dat_file="tcp_wired_$whichTerm.dat"
>$output_file
>$output_file1
>$dat_file

while [ $r -le $end ]
do
echo "total iteration: $iteration"
###############################START A ROUND
l=0;thr=0.0;del=0.0;s_packet=0.0;r_packet=0.0;d_packet=0.0;del_ratio=0.0;
dr_ratio=0.0;time=0.0;t_energy=0.0;energy_bit=0.0;energy_byte=0.0;energy_packet=0.0;total_retransmit=0.0;
num_row=5;num_col=5;num_flow=10;num_packet=0.01;

if [ "$k" == "0" ];then
	if [ "$r" == "1" ]; then
	    num_row=4;num_col=5;
	elif [ "$r" == "2" ]; then	
	    num_row=4;num_col=10;
	elif [ "$r" == "3" ]; then
	    num_row=6;num_col=10;
	elif [ "$r" == "4" ]; then
	    num_row=8;num_col=10;
	elif [ "$r" == "5" ]; then
	    num_row=10;num_col=10;
	fi 
elif [ "$k" == "5" ];then
	num_row=4;num_col=5;
	if [ "$r" == "1" ]; then
	    num_flow=10;
	elif [ "$r" == "2" ]; then	
	    num_flow=20;
	elif [ "$r" == "3" ]; then
	    num_flow=30;
	elif [ "$r" == "4" ]; then
	    num_flow=40;
	elif [ "$r" == "5" ]; then
	    num_flow=50;
	fi  
elif [ "$k" == "10" ];then
	num_row=4;num_col=5;num_flow=30;
	if [ "$r" == "1" ]; then
	    num_packet=100;
	elif [ "$r" == "2" ]; then	
	    num_packet=200;
	elif [ "$r" == "3" ]; then
	    num_packet=300;
	elif [ "$r" == "4" ]; then
	    num_packet=400;
	elif [ "$r" == "5" ]; then
	    num_packet=500;
	fi 
fi

echo "total nodes: $num_row $num_col "
echo ""


i=0
while [ $i -lt $iteration ]
do
#################START AN ITERATION
echo "                             EXECUTING $(($i+1)) th ITERATION"

num_node=$(($num_row*$num_col))
echo "flow: $num_flow"
ns wired_tcl_for_tcp.tcl $num_node $num_flow $num_packet
echo "SIMULATION COMPLETE. BUILDING STAT......"
#awk -f rule_th_del_enr_tcp.awk 802_11_grid_tcp_with_energy_random_traffic.tr > math_model1.out
awk -f wired.awk tcp_wireless.tr > tcp_wireless.out
while read val
do
	l=$(($l+1))

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
	fi


	echo "$val"
done < tcp_wireless.out

i=$(($i+1))
l=0
#################END AN ITERATION
done
total_nodes=$(($num_row * $num_col))
echo "flow is:$num_flow  num_row is:$num_row  num_col is:$num_col and speed is:$val_speed"
if [ "$k" == "0" ];then
	echo -ne "$total_nodes $thr $del $del_ratio $dr_ratio $t_energy">>$dat_file
	echo "">>$dat_file
elif [ "$k" == "5" ];then
	echo -ne "$num_flow $thr $del $del_ratio $dr_ratio $t_energy">>$dat_file
	echo "">>$dat_file
elif [ "$k" == "10" ];then
	echo -ne "$num_packet $thr $del $del_ratio $dr_ratio $t_energy">>$dat_file
	echo "">>$dat_file
fi

echo -ne "nodes: $num_row  $num_col  " >>$output_file1

echo -ne "Throughput:          $thr " >>$output_file1
echo -ne "AverageDelay:         $del " >> $output_file1
echo -ne "Sent Packets:         $s_packet " >> $output_file1
echo -ne "Received Packets:         $r_packet " >> $output_file1
echo -ne "Dropped Packets:         $d_packet " >> $output_file1
echo -ne "PacketDeliveryRatio:      $del_ratio " >> $output_file1
echo -ne "PacketDropRatio:      $dr_ratio " >> $output_file1
echo -ne "Total time:  $time " >> $output_file


r=$(($r+1))
#######################################END A ROUND
done
k=$(($k+5))
done

