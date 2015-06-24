#!/bin/bash
for i in `seq 1 65536`
do	
	A+="$RANDOM "
done
echo "$A";