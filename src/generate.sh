#!/bin/bash
for i in `seq 1 1024`
do	
	A+="$[RANDOM %10] "
done
echo "$A";
