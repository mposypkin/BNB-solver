#!/usr/bin/perl

my $i = 1;
 
print "set size square\n";
print "set xrange [-2:2]\n";
print "set yrange [-2:2]\n";

while(<>) {
 if(/step .*/) {
   print("plot 100 with lines\n");
   print("pause -1\n");
   print("unset object\n");
   $i=1;
 }
 if(/Cut type (\d).* R = (\d*\.?\d*).*centre = \[(-?\d*\.?\d*)\,(-?\d*\.?\d*)\].* box \[(-?\d*\.?\d*)\,(-?\d*\.?\d*)\]x\[(-?\d*\.?\d*)\,(-?\d*\.?\d*)\].*/) {
     print "set object $i circle at $3,$4 radius $2 fc lt $i\n";
     $i ++;
     print "set object $i rect from $5,$7 to $6,$8 fs empty border 7 fc ls 1\n";
     $i ++;
     print("plot 100 with lines\n");
     print "pause 1\n";
 }
 if(/Box:.*\[(-?\d*\.?\d*)\,(-?\d*\.?\d*)\]x\[(-?\d*\.?\d*)\,(-?\d*\.?\d*)\].*/) {
     print "set object $i rect from $1,$3 to $2,$4 fs empty border 3 fc ls 1\n";
     $i ++;
     print("plot 100 with lines\n");
     print "pause 1\n";
 }
}
