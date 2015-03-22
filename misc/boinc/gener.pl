#!/usr/bin/perl

if($#ARGV < 2) {
  print "Usage: gener.pl #point #vars\n";
  exit;
}
$x = 0.1;
$np = @ARGV[0];
$nv = @ARGV[1];
$val = 0.0;
$left = 0.0;
$right = 1.0;
for $i (1..$np) { 
  print "<solu><x>";
  for $j (1..$nv){
      $x = rand();
      $y = $left + ($right - $left) * $x;
      printf("%.3f ", $y );
  }
  printf("</x><f>%.3f</f></solu>\n", $val);
}
