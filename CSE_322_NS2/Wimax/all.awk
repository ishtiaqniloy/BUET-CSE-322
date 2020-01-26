# 初始化设定
BEGIN {
        sends = 0;
        receives = 0;
	      routing_packets = 0;
	      end_to_end_delay = 0;
	      highest_packet_id = 0;
	      recieveByte = 0;
	      drop = 0;
	      startTime = 1000000.0;
	      endTime = 0.0;
}

{action=$1;

if(action == "s" || action == "r" || action == "f" || action == "d")
{
	#nByte = $36;
	# parse the time
	if($2 == "-t")
		time = $3;
		
	#paarse the packet_id
	if($40 == "-Ii")
		packet_id = $41;
		
	#calculate the sent packets
	if(action == "s" && $19 == "AGT" && $35 == "cbr")
		sends++;
		
	#find the number of packets in the simulation
	if(packet_id > highest_packet_id)
		highest_packet_id = packet_id;
		
	#set the start time, only if its not already set
	if(start_time[packet_id] == 0)
		start_time[packet_id] = time;
		
	#calculate the receives and end-end delay
	if(action == "r" && $19 == "RTR" && $35 == "cbr")
		{	
			#printf("%d %.4f %.4f\n",nByte, time, (time + 0.0002));
			receives++;
			recieveByte += 1000;

			end_time[packet_id]= time + 0.0002;
		}
	if(time<startTime) startTime=time;
	if(time>endTime) endTime=time;
	if(action == "d" )
	{
		
		drop++;
	}
	
	
	
	#calculate the routing packets
	if(action == "s" || action == "f" && $19 == "RTR" && $35 == "AODV" || $35 == "DSR" || $35 == "message")
		routing_packets++;
}
}
# 最后输出结果
END {
	#calculate the apcket duration for all the packets
	end_to_end_delay = 0; receives = sends-4; recieveByte -= 4*1000;
	throughput = recieveByte*8/(endTime - startTime);
	for(packet_id = 0; packet_id < highest_packet_id; packet_id++)
	{
		packet_duration = end_time[packet_id] - start_time[packet_id];
	#printf("%.4f\n",packet_duration);
		if(packet_duration > 0){

			end_to_end_delay = end_to_end_delay + packet_duration;
		}
	}
	
	#calculat the average end-end packet delay
	avg_end_to_end_delay = end_to_end_delay / receives;
	
	#calculat he packet delivery fraction
	pdfraction = (receives / sends) * 100;
	dropRatio = (drop/sends) * 100;
	
	printf "cbr send:%d\nrecieve:%d\nrecieve Ratio:%.4f\nrouted packets:%d\nthroughput:%.4f\n", sends, receives, pdfraction, routing_packets, throughput;
	printf "avg_end_to_end_delay %.4f\ndropped: %d\nDrop_Ratio: %f\n", avg_end_to_end_delay,drop,dropRatio;
}

