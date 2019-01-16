BEGIN {
	
	
	max_node = 2000;
	nSentPackets = 0.0 ;		
	nReceivedPackets = 0.0 ;
	rTotalDelay = 0.0 ;
	max_pckt = 10000;
	
	idHighestPacket = 0;
	idLowestPacket = 100000;
	rStartTime = 10000.0;
	rEndTime = 0.0;
	nReceivedBytes = 0;

	for(i = 0; i < max_pckt; i++){
		send_flag[i] = 0;
	}

	for (i=0; i<max_node; i++) {
		node_thr[i] = 0;		
	}

	#printf("Starting Analysis\n");
}

{

	strEvent = $1;  
	rTime = $2;
	from_node = $3;
	to_node = $4;
	pkt_type = $5;
	pkt_size = $6;
	flgStr = $7;
	flow_id = $8;
	src_addr = $9;
	dest_addr = $10;
	#seq_no = $11;
	pkt_id = $11;

	sub(/^_*/, "", node);
	sub(/_*$/, "", node);

	
	
	if(pkt_type == "tcp"){

		if (pkt_id > idHighestPacket) idHighestPacket = pkt_id;
		if (pkt_id < idLowestPacket) idLowestPacket = pkt_id;	
		
	
		if(rTime<rStartTime) rStartTime=rTime;
		if(rTime>rEndTime) rEndTime=rTime;	
		if ( strEvent == "+" && pkt_size == "1040") {
			
			source = int(from_node)
			potential_source = int(src_addr)
			if(source == potential_source) {
				nSentPackets += 1 ;	rSentTime[ pkt_id ] = rTime ;
				send_flag[pkt_id] = 1;
			}
			
			
			
		}
		potential_dest = int(to_node)
		dest = int(dest_addr) 
		if ( strEvent == "r" && potential_dest == dest && pkt_size == "1040") {
			nReceivedPackets += 1 ;		nReceivedBytes += pkt_size;
			potential_source = int(src_addr)
			rReceivedTime[ pkt_id ] = rTime ;
			rDelay[pkt_id] = rReceivedTime[ pkt_id] - rSentTime[ pkt_id ];
			rTotalDelay += rDelay[pkt_id]; 
			node_thr[potential_source] += pkt_size;
		}
		if(strEvent == "d" && pkt_size == "1040"){
			#printf("Packet Dropped\n");
			
			nDropPackets += 1;
		}
	}
	
}

END {

	rTime = rEndTime - rStartTime ;
	rThroughput = nReceivedBytes*8 / rTime;
	rPacketDeliveryRatio = nReceivedPackets / nSentPackets * 100 ;
	rPacketDropRatio = nDropPackets / nSentPackets * 100;

	if ( nReceivedPackets != 0 ) {
		rAverageDelay = rTotalDelay / nReceivedPackets ;
	}
	printf( "%15.2f\n%15.5f\n%15.2f\n%15.2f\n", rThroughput, rAverageDelay, nSentPackets, nReceivedPackets);
	printf( "%15.2f\n%10.2f\n%10.2f\n%10.5f\n", nDropPackets, rPacketDeliveryRatio, rPacketDropRatio,rTime);
	#printf("Total Delay = %15.5f\n",rTotalDelay);
	#printf("Ending Analysis\n");

	for (i=0; i<max_node; i++) {
		if (node_thr[i] > 0) {
			printf( "%15d %15.2f\n", i, node_thr[i]*8 / rTime );
		}
	}


}


