#!/bin/sh
lockfile /tmp/bnblock
echo mpirun -maxtime $4 -np $1 $BNBHOME/apps/bnbsolver.exe \"-s $2 $3\"
n=$(((($1 - 1)/8) + 1))
h=$(($4/60))
m=$(($4-$h*60))
pbsf=`date +%d.%H.%M.pbs`
echo \#PBS -l walltime=$h:$m:00,nodes=$n:ppn=8 >$pbsf
echo \#PBS -q workq@master >>$pbsf
echo \#PBS -N bnbsolver.exe >>$pbsf
echo \#!/bin/sh >>$pbsf
echo cd $BNBHOME >>$pbsf
echo lamboot >>$pbsf
echo mpiexec -np $1 ./apps/bnbsolver.exe -s $2 $3 >>$pbsf
echo wipe >>$pbsf
qsub $pbsf
rm -f /tmp/bnblock
