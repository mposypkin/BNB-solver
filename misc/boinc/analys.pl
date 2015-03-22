#!/usr/bin/perl 
my @arr = ();
if($#ARGV >= 0) {
  $fol = $ARGV[0];
} else {
  die "supply folder name as a command line argument\n";
}
$K = -1;
while(defined($fn = <$fol/wu*0>)) {
    $K ++;
    open(F, $fn);
    while(<F>) {
      if($_ =~ ".*<f>(.*)<\/f>.*") {
	@arr = (@arr, $1);
      }
    }
    close(F);
}
my @brr = sort {$a <=> $b} @arr;
$sum = 0;
for(@brr) {$sum += $_;}
$n = $#brr + 1;
$av = $sum / $n;
$mine = $brr[0];
$maxe = $brr[$#brr];
print "$n elements, min = $mine, max = $maxe, average = $av\n";
