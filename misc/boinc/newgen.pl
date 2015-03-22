#!/usr/bin/perl 

# extracts new generation from the existing results

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
	print $_;
      }
    }
    close(F);
}
