#!/bin/sh
#SBATCH --job-name=dasa2_assignment3
#SBATCH --mail-type=ALL
#SBATCH --mail-user=anirban.dash@gmail.com

NTPERNODE=64
TIME=20

srun -o result_64_1.log --overcommit --ntasks-per-node ${NTPERNODE} --nodes 1 --partition debug --time ${TIME} ./assignment3_debug 
srun -o result_128_2.log --overcommit --ntasks-per-node ${NTPERNODE} --nodes 2 --partition debug --time ${TIME} ./assignment3_debug 
srun -o result_256_4.log --overcommit --ntasks-per-node ${NTPERNODE} --nodes 4 --partition debug --time ${TIME} ./assignment3_debug 
srun -o result_512_8.log --overcommit --ntasks-per-node ${NTPERNODE} --nodes 8 --partition debug --time ${TIME} ./assignment3_debug 
srun -o result_1024_16.log --overcommit --ntasks-per-node ${NTPERNODE} --nodes 16 --partition debug --time ${TIME} ./assignment3_debug 
srun -o result_2048_32.log --overcommit --ntasks-per-node ${NTPERNODE} --nodes 32 --partition debug --time ${TIME} ./assignment3_debug 
srun -o result_4096_64.log --overcommit --ntasks-per-node ${NTPERNODE} --nodes 64 --partition small --time ${TIME} ./assignment3 
srun -o result_8192_128.log --overcommit --ntasks-per-node ${NTPERNODE} --nodes 128 --partition medium --time ${TIME} ./assignment3 