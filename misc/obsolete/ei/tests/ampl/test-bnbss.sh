#!/bin/sh
#SUPERVISOR="/usr/bin/time -p -a"
#SUPERVISOR="gdb --args"
ROOT=../../..
BNBSS=$ROOT/ei/apps/bnbss.exe

TYPE="$1"
if test "$1" = "" ; then
	TYPE=builtin
fi

$SUPERVISOR $BNBSS ./$TYPE.scr ./results-$TYPE.txt
RES=$?
rm -f ./results-$TYPE.txt
exit $RES
