#!/bin/bash
:	'
node_arr=( 20 40 60 80 100 )
flow_arr=( 10 20 30 40 50 )
pkt_arr=( 100 200 300 400 500 )


temp_arr=("${pkt_arr[@]}")

for i in ${temp_arr[*]}
do
	echo $i
done
'
declare -A hashtbl
prev_val=0.0
flag=0
sum=0
count=1
node_val=10
while IFS=: read -r line
do
	#echo $line
	k=0
	for word in $line
	do 
		k=$(($k+1))
		if [ "$k" == "1" ]; then
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
				avg_val=$(echo "scale=5; $sum/$count" | bc)
				hashtbl["$prev_val"]="$avg_val"
				#echo "Change at $prev_val"
				echo "time = $prev_val, avg_val = $avg_val"
				sum=0
				count=1
				prev_val=$word
			else
				flag=0
				#node_val=`echo $node_val + 0.0 | bc`
				#echo "$count and $node_val at $word"
				
			fi
		elif [ "$k" == "2" ]; then
			if [ $flag -eq 0 ];then 
				#echo "current count= $count, changing it"
				count=$(($count+1))
			fi
		else
			#echo $word
			sum=`echo $sum + $word | bc`
			#sum=$(( $sum + $word )) 
		fi

	done
	

	
done < out_v2/congestionData.dat
echo ${hashtbl[*]}
echo "${#hashtbl[@]}"

echo ${hashtbl["2.30"]}


: '
var2="Congestion_value"
			output_file="$folder$output_file_format$under$var$under$var2$under$all.out"
			prev_val=0.0
			flag=0
			sum=0
			count=1
			conges_avg=0.0
			flag2=0
			echo "============= $i iteration data ============" >> "$output_file"
			while IFS=: read -r line
			do
				#echo $line
				k=0
				for word in $line
				do 
					k=$(($k+1))
					if [ "$k" == "1" ]; then
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
							conges_avg=$(echo "scale=5; $conges_avg+$avg_node_conges/$iteration_float" | bc)
							#echo "time = $prev_val, avg_val = $avg_val"
							sum=0
							count=1
							prev_val=$word
						else
							flag=0
							if [ $flag2 -eq 0 ] && [ "$word" == "2.4000000000000008" ];then
								echo "What is wrong in here"
								flag2=1
							fi
							
						fi
					elif [ "$k" == "2" ]; then
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
			echo "count= $count node = $node"
			if [ "$count" == "$node" ]; then
				echo "Last values processing" 
				$time=$(echo "scale=2; $time/1.0" | bc)
				avg_val=$(echo "scale=5; $sum/$count" | bc)
				echo "Time = $time congestion ratio: $avg_node_conges " >> "$output_file"
			fi
			k=0
'