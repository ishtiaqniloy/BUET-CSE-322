# wired.tcl
# A simple example for wired simulation
# ======================================================================
# Define variables
# ======================================================================
if {$argc != 3} {
	puts "Please input in correct order <file_name> <n_nodes> <n_flow> <n_pkts_p_sec>"
	exit 1;
}


set factor 1
set num_node  [lindex $argv 0];# 20  [expr 20*$factor]
set num_flow  [lindex $argv 1];# 10  [expr 10*$factor]
set x_dim 1000
set y_dim 1000
set queue_length 30

set start_time 1.0
set time_duration 1.4
set extra_time 0.1

set num_packets_per_sec [lindex $argv 2]; # [expr 100*$factor]
set cbr_size 1000
set cbr_rate 11.0Mb
set cbr_interval [expr 1.0/$num_packets_per_sec];

set row_val 5; #half number of 
set col_val [expr $num_node/$row_val];

set tcp_src Agent/TCP ;# Agent/TCP or Agent/TCP/Reno or Agent/TCP/Newreno or Agent/TCP/FullTcp/Sack or Agent/TCP/Vegas
set tcp_sink Agent/TCPSink ;# Agent/TCPSink or Agent/TCPSink/Sack1

$tcp_src set windowOption_ 1

set nm out/tcp_wired.nam
set tr tcp_wired.tr
set topo_file out/topo_tcp_wired.txt
set conges out/congestionData.dat


create-god $num_node
# ======================================================================
# Main Program
# ======================================================================


# Initialize ns
#
set ns [new Simulator]

$ns rtproto DV
set tracefd [open $tr w]
$ns trace-all $tracefd


set namtrace [open $nm w]
$ns namtrace-all $namtrace

set topofile [open $topo_file "w"]

# set up topography object
set topo       [new Topography]
$topo load_flatgrid $x_dim $y_dim

set congestionFile [open $conges "w"]


puts "start node creation"
for {set i 0} {$i < $num_node} {incr i} {
	set node_($i) [$ns node]

}

puts "node creation done"

#Node Positioning Not done

set i 0
while {$i < $num_node } {

	#Set random position for nodes
	set x_position [expr int($x_dim*rand())] ;#random settings
	set y_position [expr int($y_dim*rand())] ;#random settings

	$node_($i) set X_ $x_position
	$node_($i) set Y_ $y_position
	$node_($i) set Z_ 0.0

	puts -nonewline $topofile "$i x: [$node_($i) set X_] y: [$node_($i) set Y_] \n"
	incr i;
}; 

puts "node positioning done"

puts "start link creation"

for {set i 0} {$i < $num_node} {incr i} {
	set source_end $i 
	set dest_end [expr ($i+1)%$num_node]
	$ns duplex-link $node_($source_end) $node_($dest_end) 2Mb 10ms DropTail
	$ns queue-limit $node_($source_end) $node_($dest_end) $queue_length
	puts -nonewline $topofile "Link source_end = $source_end and destination_end = $dest_end\n"
}

puts "link creation done, starting flow creation"
#random flow
for {set i 0} {$i < $num_flow} {incr i} {
	set source [expr int($num_node*rand())]
	set destination $source
	while {$source == $destination} {
		set destination [expr int($num_node*rand())]
	}

	set tcp($i) [new $tcp_src]
	$tcp($i) set class_ $i
	$tcp($i) set fid_ $i
	set sink($i) [new $tcp_sink]

	$ns attach-agent $node_($source) $tcp($i)
	$ns attach-agent $node_($destination) $sink($i)

	$ns connect $tcp($i) $sink($i)



	if {[expr $i%2 == 0]} {
		$ns color $i blue
	} else {
		$ns color $i red
	}

	puts -nonewline $topofile "Flow source = $source and destination = $destination\n"

}
puts "Link creation completed"
#setup CBR over tcp
for {set i 0} { $i < $num_flow } { incr i } {
	set cbr_($i) [new Application/Traffic/CBR]
	$cbr_($i) set packetSize_ $cbr_size
	$cbr_($i) set rate_ $cbr_rate
	$cbr_($i) set interval_ $cbr_interval
	$cbr_($i) attach-agent $tcp($i)



}
puts "CBR Invoked, Event Setup"

#Congestion Window Data

proc plotting {tcpsrc plotterfile} {
	global ns
	set congestion [$tcpsrc set cwnd_]
	set id [$tcpsrc set fid_]
	set now [$ns now]
	puts $plotterfile "$now $id $congestion"

	$ns at [expr $now+0.1] "plotting $tcpsrc $plotterfile"
	
}

for {set i 0} {$i < $num_flow} {incr i} {
	$ns at 0.2 "plotting $tcp($i) $congestionFile"
}


#Event Scheduling

for {set i 0} {$i < $num_flow} {incr i} {
	$ns at $start_time "$cbr_($i) start"
	
}

for {set i 0} {$i < $num_flow} {incr i} {
	$ns at [expr $start_time+$time_duration] "$cbr_($i) stop"
}

$ns at [expr $start_time+$time_duration/2] "puts \"half of the simulation is finished\""
$ns at [expr $start_time+$time_duration] "puts \"end of simulation duration\""

$ns at [expr $start_time+$time_duration+$extra_time] "finish"


for {set i 0} {$i < $num_node } { incr i} {
	#$ns initial_node_pos $node_($i) 4
}



# ======================================================================
# Finishing
# ======================================================================


proc finish {} {
	puts "finishing Simulation"
	global ns tracefd namtrace topofile nm

	$ns flush-trace
	close $tracefd
	close $namtrace
	close $topofile
	#exec nam $nm &
        exit 0


}

# ======================================================================
# Run Simulation
# ======================================================================

$ns run

