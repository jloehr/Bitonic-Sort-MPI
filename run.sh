#!/bin/bash

DATE=`date +%Y-%m-%d_%H-%M-%S`

mpiexec -n 8 ./bin/bitonic-mpi -t 65536 -n 1 -f ./data/65536tweets -o ./out/  "la" > ./out/log
#/usr/bin/time -l mpiexec -n 2 ./bin/bitonic-mpi -t 2097152 -n 8 -f ./data/2097152tweets  "la"
#mpiexec -n 4 ./bin/bitonic-mpi -t 2097152 -n 1 -f ./data/2097152tweets  "la"
#./bin/bitonic-mpi -t 65536 -n 1 -f ./data/65536tweets  "la"
#./bin/bitonic-mpi -t 2097152 -n 8 -f ./data/2097152tweets  "la"
