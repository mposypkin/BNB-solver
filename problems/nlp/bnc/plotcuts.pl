#!/usr/bin/perl

my $i = 1;
 
print "set size square\n";
print "set xrange [-2:2]\n";
print "set yrange [-2:2]\n";

while(<>) {
 chomp;
 if(/step (\d+) .*/) {
   print "plot 100 with lines\n";
   print "print \"Press Enter to proceed with step $1\"\n";
   print "pause -1\n";
   print "unset object\n";
   $i=1;
 }
 if(/Cut type (\d).* R = (\d*\.?\d*).*centre = \[([^\,]*)\,([^\,]*)\].* box \[([^\,]*)\,([^\,]*)\]x\[([^\,]*)\,([^\,]*)\].*/) {
     print "print \"$_\"\n";
     print "set object $i circle at $3,$4 radius $2 fc lt $i\n";
     $i ++;
     #print "set object $i rect from $5,$7 to $6,$8 fs empty border 7 fc ls 1\n";
     my $n5 = $5 - 0.01;
     my $n7 = $7 - 0.01;
     my $n6 = $6;
     my $n8 = $8;
     print "set object $i rect from $n5,$n7 to $n6,$n8 fs empty border 7 fc ls 1\n";

     $i ++;
     print("plot 100 with lines\n");
     print "pause 1\n";
 }
 if(/.*Resulting.*/) {
     print "print \"Resulting boxes:\"\n";
 }
 if(/Box:.*\[(-?\d*\.?\d*)\,(-?\d*\.?\d*)\]x\[(-?\d*\.?\d*)\,(-?\d*\.?\d*)\].*/) {
     print "print \"$_\. Wait 1 second.\"\n";
     #print "set object $i rect from $1,$3 to $2,$4 fs empty border 3 fc ls 1\n";
     my $n1 = $1 - 0.01;
     my $n3 = $3 - 0.01;
     my $n2 = $2;
     my $n4 = $4;
     print "set object $i rect from $n1,$n3 to $n2,$n4 fs empty border 3 fc ls 1\n";
     $i ++;
     print("plot 100 with lines\n");
     print "pause 1\n";
 }
}
