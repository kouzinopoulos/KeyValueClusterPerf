#!/bin/bash
# This script runs multiple simulations in a certain configuration by changing the amount of hosts

echo "start"

# first get password for private key
#ssh-agent bash
ssh-add ~/.ssh/id_rsa

# first argument contains the startnumber of hosts
#startHost=$1

# second argument contains the endnumber of hosts
#endHost=$2

# third argument contains stepsize
#stepsize=$3
#1 10 30 50 70 100 200 400 600 800 1000
numberofhosts=(1 10 100 200) 
# 10000 100000 1000000 10000000
datasizes=(1000 10000 100000  1000000 10000000 50000000)
simit=1

#iterations=(1 2 3 4 5)

#for iter in "${iterations[@]}"; do
# loop accross all data sizes
for datasize in "${datasizes[@]}"; do
	printf "valueDistributionType=Constant\nsize="$datasize > ~/Desktop/configuration/valueDistribution.cfg
	ansible-playbook ~/Desktop/Scripts/config-valuedistribution.yml -u root
	for i in "${numberofhosts[@]}"; do
		# start the performance cluster nodes
		ansible-playbook ~/Desktop/Scripts/cmd-startkeyvalueclusterperf.yml -u root
		# start the controller for this iteration
		echo ssh root@cernvmbl023 export LD_LIBRARY_PATH=/usr/local/lib; ./keyvalueclusterperf --controller --hostlimit $i --simiteration $simit
		let "simit+=1"
		# kill all remaining processes on performance cluster
		ansible-playbook ~/Desktop/Scripts/cmd-stopkeyvalueclusterperf.yml -u root
		# let the user know the simiteration
		echo "Currently doing datasize: " $datasize
		echo "Currently finished with: " $i
		#echo "Currently in iteration:" $iter
	done
done
#done
