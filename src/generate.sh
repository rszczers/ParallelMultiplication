#!/bin/sh
for((i=0; i<$1 * $1; i++)); do
    printf "%s" "$[RANDOM % 10] " >> $2 
done
