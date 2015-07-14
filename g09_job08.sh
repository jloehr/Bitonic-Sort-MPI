#!/bin/bash
#PBS -j oe
#PBS -l feature=mpp
#PBS -l nodes=8:ppn=1

TWEETSPERFILE=2097152
NUMBEROFFILES=8
SEARCHTERM="la"
DATE=`date +%Y-%m-%d_%H-%M-%S`

cd $PBS_O_WORKDIR

#create output directory
mkdir -p ./out/

aprun -B ./bin/bitonic-mpi -t $TWEETSPERFILE -n $NUMBEROFFILES -f ./../${TWEETSPERFILE}tweets -o ./out/  $SEARCHTERM > ./out/log.$DATE