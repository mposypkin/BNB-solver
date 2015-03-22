#!/bin/bash
export PATH=$PATH:/home/bagside/utils/mpich/bin
if test $# -le 3
then
  echo AA
  echo $PATH
  which mpirun
  echo OO mpirun -np $1 $BNBHOME/apps/bnbsolver.exe -s $2 $3
  mpirun -np $1 $BNBHOME/apps/bnbsolver.exe -s $2 $3&
else
  lockfile /tmp/bnblock
  echo mpirun -maxtime $4 -np $1 $BNBHOME/apps/bnbsolver.exe \"-s $2 $3\"
  mpirun -maxtime $4 -np $1 $BNBHOME/apps/bnbsolver.exe "-s $2 $3"
  rm -f /tmp/bnblock
fi
