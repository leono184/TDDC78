$#!/bin/sh
set -e

$ ssh -X x_anste@triolith.nsc.liu.se

$ module load buildenv-intel/2015-1

$ make 

$ module unload buildenv-intel/2015-1

for n in 0:4; do
	echo "cores: $n" >> "result$n.txt"
	#mpirun -n $n $1 $2 "../images/im$3.ppm" $4  >> "result$3.txt"
	$#SBATCH -J thres
	$#SBATCH -t 00:05:00
	$#SBATCH -n $n^2
	$#SBATCH -o thres_out$n
	$#SBATCH -e thres_err$n
	
	mpprun thres
done


