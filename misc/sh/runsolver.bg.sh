#!/bin/sh
  echo STARTING BNBSOLVER
  n=$(((($1 - 1)/4) + 1))
  h=$(($4/60))
  m=$(($4-$h*60))
  lockfile /tmp/bnblock
  cp $BNBHOME/apps/bnbsolver.exe /gpfs/data/posypkin
  cd /gpfs/data/posypkin
  echo mpisubmit.bg -n $n -w $h:$m:00 -m vn -g allowed256 ./bnbsolver.exe -- -s $2 $3
  mpisubmit.bg -n $n -w $h:$m:00 -m vn -g allowed256 ./bnbsolver.exe -- -s $2 $3
  rm -f /tmp/bnblock

