#!/bin/bash
make -f Makefile
#~/UGP/allocator/src/allocator.out 64 8
for execution in 0 1 2 3 4
do
	for P in 16 36 49 64
	do	
		for N in 256 1024 4096 16384 65536 262144 1048576
		do
			echo "$execution, $P, $N\n"
			mpirun -np $P -f hosts ./halo $N 50
		done
	done
done