

#INPUT: output file AND number of iterations
output_file_format="tcp";
iteration_float=5.0;

start=5
end=5

node=20
flow=10
npkt=100

node_arr=( 20 40 60 80 100 )
flow_arr=( 10 20 30 40 50 )
pkt_arr=( 100 200 300 400 500 )

variation=3
var=1

iteration=$(printf %.0f $iteration_float);

r=$start

while [ $r -le $end ]
do

#while [ $var -le $variation]
#do

echo "total iteration: $iteration"
###############################START A ROUND
l=0;thr=0.0;del=0.0;s_packet=0.0;r_packet=0.0;d_packet=0.0;del_ratio=0.0;
dr_ratio=0.0;time=0.0;t_energy=0.0;energy_bit=0.0;energy_byte=0.0;energy_packet=0.0;total_retransmit=0.0;

i=0
while [ $i -lt $iteration ]
do
#################START AN ITERATION
echo "                             EXECUTING $(($i+1)) th ITERATION"

ns wired_tcl_for_tcp.tcl $node $flow $npkt
echo "SIMULATION COMPLETE. BUILDING STAT......"
#awk -f rule_th_del_enr_tcp.awk 802_11_grid_tcp_with_energy_random_traffic.tr > math_model1.out
awk -f tcp_wired_sam.awk tcp_wired.tr > tcp_wired.out

while read val
do
#	l=$(($l+$inc))
	l=$(($l+1))

	dir="/home/shashata/Documents/NS2/ns temp/TemporaryRepo/wired_tries/"
	#dir=""
	under="_"
	all="all"
	output_file="$dir$output_file_format$under$r$under$r$under$all.out"
	
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
done < tcp_wired.out

i=$(($i+1))
l=0
#################END AN ITERATION
done

dir="/home/shashata/Documents/NS2/ns temp/TemporaryRepo/wired_tries/"
#dir=""
under="_"
output_file="$dir$output_file_format$under$r$under$r.out"

echo -ne "Throughput:          $thr " > "$output_file"
echo -ne "AverageDelay:         $del " >> "$output_file"
echo -ne "Sent Packets:         $s_packet " >> "$output_file"
echo -ne "Received Packets:         $r_packet " >> "$output_file"
echo -ne "Dropped Packets:         $d_packet " >> "$output_file"
echo -ne "PacketDeliveryRatio:      $del_ratio " >> "$output_file"
echo -ne "PacketDropRatio:      $dr_ratio " >> "$output_file"
echo -ne "Total time:  $time " >> "$output_file"

r=$(($r+1))

#done
#######################################END A ROUND
done
