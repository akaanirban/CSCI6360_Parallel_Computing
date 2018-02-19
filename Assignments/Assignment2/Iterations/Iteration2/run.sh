#!/bin/bash

echo "Compiling using  mpicc -g -Wall mpi-cla-io.c -o mpi-cla-io"
mpicc -g -Wall mpi-cla-io.c -o mpi-cla-io

echo "Running the C program using :mpirun -np 4 ./mpi-cla-io $1 $2 "
mpirun -np $4 ./mpi-cla-io $1 $2

echo "Parsing the program in to ActualOutput.txt and MyOutput.txt"
python3 textDiff.py $2 $3
