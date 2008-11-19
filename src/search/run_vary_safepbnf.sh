#!/bin/bash

for min in 5 15 30 50 80
do
    for nblocks in 1 5 10 15 20
    do
	for (( threads=1; threads <= 10; threads++ ))
	do
	    ./run_grids.sh safepbnf \
		-m $min \
		-t $threads \
		-n $nblocks
	done
    done
done
