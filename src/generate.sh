#!/bin/sh
for((i=0; i<$1 * $1; i++)); do
	A+="$[RANDOM % 10] "
done
echo "$A";
