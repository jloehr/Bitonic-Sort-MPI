#mpiexec -n 1 ./bin/bitonic-mpi -t 65536 -n 2 -f ./data/65536tweets  "la"
#mpiexec ./bin/bitonic-mpi -t 2097152 -n 2 -f ./data/2097152  "la"
#./bin/bitonic-mpi -t 65536 -n 1 -f ./data/65536tweets  "la"
./bin/bitonic-mpi -t 2097152 -n 1 -f ./data/2097152  "la"