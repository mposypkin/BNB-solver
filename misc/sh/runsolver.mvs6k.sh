#!/bin/sh
lockfile /tmp/bnblock.mvs6k
echo mpirun -maxtime $4 -np $1 $BNBHOME/apps/bnbsolver.exe \"-s $2 $3\"
mpirun -maxtime $4 -np $1 $BNBHOME/apps/bnbsolver.exe "-s $2 $3"
rm -f /tmp/bnblock.mvs6k	
