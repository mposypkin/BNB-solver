#!/bin/sh
lockfile /tmp/bnblock.mvs100k
IP=`cat $HOME/IP/mvs100k`
echo mpirun -maxtime $4 -np $1 $BNBHOME/apps/bnbsolver.exe \"-s $IP $3\"
mpirun -maxtime $4 -np $1 $BNBHOME/apps/bnbsolver.exe "-s $IP $3"
echo sleeping ...
sleep 180
rm -f /tmp/bnblock.mvs100k
