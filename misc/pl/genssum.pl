#!/usr/bin/perl

if($#ARGV < 1) {
  print "Usage: genssum.pl #nitems #ampl\n";
  exit;
}
$x = 0.1;
$n = @ARGV[0];
$ampl = @ARGV[1];
$sd = 3;
for $i (1..$n) { 
  $x = 2 * int(rand() * $ampl);
  push(@w, $x);
}
@nw = sort {$b <=> $a} @w;
print "<n>$n</n>\n";
print "<w> ";
foreach(@nw) {
  print "$_ ";
}
print "</w>\n";
print "<p> ";
foreach(@nw) {
  print "$_ ";
}
print "</p>\n";
print "<c>C</c>\n";
