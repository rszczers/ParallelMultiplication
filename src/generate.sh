#!/bin/bash
for i in `seq 1 65536`
do	
	A+="$[RANDOM % 10] "
done
echo "$A";
