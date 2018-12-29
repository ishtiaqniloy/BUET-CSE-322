#cd /
#cd home
#cd ubuntu
#cd ns2\ programs/

#INPUT: output file AND number of iterations
output_file_format="tcp";
iteration_float=10.0;

start=5
end=5


iteration=$(printf %.0f $iteration_float);

r=$start

while [ $r -le $end ]
do
echo "total iteration: $iteration"
###############################START A ROUND
l=0;thr=0.0;del=0.0;s_packet=0.0;r_packet=0.0;d_packet=0.0;del_ratio=0.0;
dr_ratio=0.0;time=0.0;t_energy=0.0;energy_bit=0.0;energy_byte=0.0;energy_packet=0.0;total_retransmit=0.0;

i=0
while [ $i -lt $iteration ]
do
#################START AN ITERATION
echo "                             EXECUTING $(($i+1)) th ITERATION"

ns tcp_design_for_802_11.tcl $r
echo "SIMULATION COMPLETE. BUILDING STAT......"
#awk -f rule_th_del_enr_tcp.awk 802_11_grid_tcp_with_energy_random_traffic.tr > math_model1.out
awk -f /home/shashata/Documents/NS2/nsSir/ns/ns2\ programs/itcp/rule_tcp_wireless.awk /home/shashata/Documents/NS2/nsSir/ns/ns2\ programs/raw_data/tcp_wireless.tr > /home/shashata/Documents/NS2/nsSir/ns/ns2\ programs/raw_data/tcp_wireless.out

while read val
do
#	l=$(($l+$inc))
	l=$(($l+1))

	dir="/home/shashata/Documents/NS2/nsSir/ns/ns2_data/iTCP/"
	#dir=""
	under="_"
	all="all"
	output_file="$dir$output_file_format$under$r$under$r$under$all.out"
	
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
	fi


	echo "$val"
done < /home/shashata/Documents/NS2/nsSir/ns/ns2\ programs/raw_data/tcp_wireless.out

i=$(($i+1))
l=0
#################END AN ITERATION
done

dir="/home/shashata/Documents/NS2/nsSir/ns/ns2_data/iTCP/"
#dir=""
under="_"
output_file="$dir$output_file_format$under$r$under$r.out"

echo -ne "Throughput:          $thr " > $output_file
echo -ne "AverageDelay:         $del " >> $output_file
echo -ne "Sent Packets:         $s_packet " >> $output_file
echo -ne "Received Packets:         $r_packet " >> $output_file
echo -ne "Dropped Packets:         $d_packet " >> $output_file
echo -ne "PacketDeliveryRatio:      $del_ratio " >> $output_file
echo -ne "PacketDropRatio:      $dr_ratio " >> $output_file
echo -ne "Total time:  $time " >> $output_file
echo -ne "Total energy consumption:        $t_energy " >> $output_file
echo -ne "Average Energy per bit:         $energy_bit " >> $output_file
echo -ne "Average Energy per byte:         $energy_byte " >> $output_file
echo -ne "Average energy per packet:         $energy_packet " >> $output_file
echo "total_retransmit:         $total_retransmit " >> $output_file

r=$(($r+1))
#######################################END A ROUND
done
