#ns parameters format:
#ns <filename.tcl> #row #col #flows #packets_per_sec #speed_nodes

#default values
def_num_row=6
def_num_col=10
def_num_flows=30
def_packets_per_sec=300.0
def_speed_nodes=15

#maximum values
max_node_num=100
max_num_flows=50
max_packets_per_sec=500.0
max_speed_nodes=25

#initial values
num_row=5
num_col=8
num_flows=10
packets_per_sec=100.0
speed_nodes=5

cd /
cd home
cd ishtiaq/ns
cd ns2\ programs/

#INPUT: output file AND number of iterations
output_file_format="tcp";
iteration_float=3.0;

dir="/home/ishtiaq/ns/ns2_data/iTCP/"
#dir=""
under="_"
graph="GRAPH"
all="all"

iteration=$(printf %.0f $iteration_float);

############################################################VARYING NUMBER OF NODES

node_num=$(($num_row * $num_col))
vary="NodeNumVary"

#deleting previous files
output_file="$dir$output_file_format$under$vary.out"
rm -f $output_file
touch $output_file

output_file2="$dir$output_file_format$under$vary.out"
rm -f $output_file2
touch $output_file2

while [ $node_num -le $max_node_num ]
do
###############################START A ROUND
echo "total iteration: $iteration"
l=0;thr=0.0;del=0.0;s_packet=0.0;r_packet=0.0;d_packet=0.0;del_ratio=0.0;
dr_ratio=0.0;time=0.0;t_energy=0.0;energy_bit=0.0;energy_byte=0.0;energy_packet=0.0;total_retransmit=0.0;


	output_file="$dir$output_file_format$under$all.out"
	rm -f $output_file
	touch $output_file

i=0
while [ $i -lt $iteration ]
do
#################START AN ITERATION
echo "                             EXECUTING $(($i+1)) th ITERATION"

#echo -ne "iteration: $i " >> $output_file

ns /home/ishtiaq/ns/ns2\ programs/itcp/tcp_design_for_802_11.tcl $num_row $num_col $def_num_flows $def_packets_per_sec $def_speed_nodes
echo "SIMULATION COMPLETE. BUILDING STAT......"
#awk -f rule_th_del_enr_tcp.awk 802_11_grid_tcp_with_energy_random_traffic.tr > math_model1.out
awk -f /home/ishtiaq/ns/ns2\ programs/itcp/rule_tcp_wireless.awk /home/ishtiaq/ns/ns2\ programs/raw_data/tcp_wireless.tr > /home/ishtiaq/ns/ns2\ programs/raw_data/tcp_wireless.out

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
	fi


	echo "$val"
done < /home/ishtiaq/ns/ns2\ programs/raw_data/tcp_wireless.out

i=$(($i+1))
l=0
#################END AN ITERATION
done

output_file="$dir$output_file_format$under$vary.out"
output_file2="$dir$output_file_format$under$vary.out"

#node_num=$(($num_row * $num_col))

echo -ne "Number of Nodes:			$node_num " >> $output_file
echo -ne "$node_num " >> $output_file2

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

#r=$(($r+1))

		if  [ $num_row -eq 4 ] && [ $num_col-eq 5 ]; then 
			num_row=5
			num_col=8
			echo "*********ROUND 1 COMPLETE*********"

		elif [ $num_row -eq 5 ] && [ $num_col -eq 8 ]; then
			num_row=6
			num_col=10
			echo "*********ROUND 2 COMPLETE*********"

		elif [ $num_row -eq 6 ]; then
			num_row=8
			echo "*********ROUND 3 COMPLETE*********"

		elif [ $num_row -eq 8 ]; then
			num_row=10
			echo "*********ROUND 4 COMPLETE*********"
		
		else
			num_row=100
			num_col=100
			echo "*********ROUND 5 COMPLETE*********"
		fi

		node_num=$(($num_row * $num_col))
		echo node_num $node_num

#######################################END A ROUND
done
