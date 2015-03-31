#!/usr/bin/perl

my $i = 1;
 
print "set size square\n";
print "set xrange [-3:3]\n";
print "set yrange [-3:3]\n";

while(<>) {
 if(/step .*/) {
   print("plot 100 with lines\n");
   print("pause -1\n");
 }
 if(/Box:.*\[(-?\d*\.?\d*)\,(-?\d*\.?\d*)\]x\[(-?\d*\.?\d*)\,(-?\d*\.?\d*)\].*/) {
     print "set object $i rect from $1,$3 to $2,$4 fs empty border 3 fc ls 1\n";
     $i ++;
 }
}
