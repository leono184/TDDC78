#!/bin/sh
set -e

for n in {1,2,4,8}; do
	echo "cores: $n radius: $2" >> "result$3.txt"
	mpirun -n $n $1 $2 "../images/im$3.ppm" $4  >> "result$3.txt"
done


