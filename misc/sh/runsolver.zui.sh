#!/bin/sh
cd ~/run
lockfile /tmp/bnblock
echo mpirun -maxtime $4 -np $1 $BNBHOME/apps/bnbsolver.exe \"-s $2 $3\"
h=$(($4/60))
m=$(($4-$h*60))
pbsf=`date +%d.%H.%M.sh`
echo \#!/bin/sh >$pbsf
echo \#SBATCH -n $1 >> $pbsf
echo \#SBATCH -o %j.out >> $pbsf
echo \#SBATCH -e %j.err >> $pbsf
echo \#SBATCH -t $h:$m:00 >> $pbsf
echo \#SBATCH -t $h:$m:00 >> $pbsf
echo source /opt/cluster >> $pbsf
echo willuse mpi intel-compilers >> $pbsf
echo cd  ~/run >>$pbsf
echo mpisub $BNBHOME/apps/bnbsolver.exe -s $2 $3 >>$pbsf
sbatch $pbsf
rm -f /tmp/bnblock

