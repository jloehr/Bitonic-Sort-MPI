#!/bin/bash

TWEETSPERFILE=2097152
NUMBEROFFILES=8
SEARCHTERM="la"
DATE=`date +%Y-%m-%d_%H-%M-%S`

#create output directory
mkdir -p ./out/

mpiexec -n 1 ./bin/bitonic-mpi -t $TWEETSPERFILE -n $NUMBEROFFILES -f ./../${TWEETSPERFILE}tweets -o ./out/  $SEARCHTERM > ./out/log.$DATE

#mpiexec -n 4 --output-filename ./out/log.$DATE ./bin/bitonic-mpi -t 65536 -n 1 -f ./data/65536tweets -o ./out/  "la"
#/usr/bin/time -l mpiexec -n 2 ./bin/bitonic-mpi -t 2097152 -n 8 -f ./data/2097152tweets  "la"
#mpiexec -n 2 ./bin/bitonic-mpi -t 2097152 -n 1 -f ./data/2097152tweets  "la"
#./bin/bitonic-mpi -t 65536 -n 1 -f ./data/65536tweets  "la"
#./bin/bitonic-mpi -t 2097152 -n 8 -f ./data/2097152tweets  "la"
