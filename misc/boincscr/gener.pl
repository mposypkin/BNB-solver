#!/usr/bin/perl

if($#ARGV < 2) {
  print "Usage: gener.pl #point #vars value\n";
  print "Or: gener.pl #point #vars value left right\n";
  exit;
}
$x = 0.1;
$np = @ARGV[0];
$nv = @ARGV[1];
$val = @ARGV[2];
$left = 0.0;
$right = 1.0;
if($#ARGV == 4) {
  $left = @ARGV[3];
  $right = @ARGV[4];
}
for $i (1..$np) { 
  print "<solu><x>";
  for $j (1..$nv){
      $x = rand();
      $y = $left + ($right - $left) * $x;
      printf("%.3f ", $y );
  }
  printf("</x><f>%.3f</f></solu>\n", $val);
}
