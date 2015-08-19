#!/bin/bash
# declare the base portnumber
export LD_LIBRARY_PATH=/usr/local/lib
basePort=3000
dbasePort=4000
echo "Starting " $1 " instances"
for (( i=0; i<$1; i++)); do
	let "port=$basePort+$i"
	let "dport=$dbasePort+$i"
	echo $port
	./keyvalueclusterperf --port $port --dport $dport --skipinitialisation | tee ~/log/$i.log &
done

# use 'pkill -f keyvalueclusterperf' to kill any processes that are not shut down safely