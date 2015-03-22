#!/bin/sh
ROOT=../../..
BNBSS=$ROOT/ei/apps/bnbss.exe

rm -f tmp.txt
$BNBSS ./mors20.scr ./tmp.txt
if grep ASYNC ./tmp.txt; then echo Ok; else exit 1; fi
rm tmp.txt
$BNBSS ./mors20_1.scr ./tmp.txt
if grep ASYNC ./tmp.txt; then echo Ok; else exit 1; fi
rm tmp.txt

