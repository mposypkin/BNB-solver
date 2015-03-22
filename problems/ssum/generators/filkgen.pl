#!/usr/bin/env perl

if(@ARGV != 2) {
  die "Usage: fingen.pl #items #fraction for the bound";
}
my $N = @ARGV[0];
my $frac = @ARGV[1];
my $a = 2;
my $k = int($N / $frac);
print $N, " ";
print $a * $k + 1, " ";
for($i = 0; $i < $N; $i ++) {
  print $a, " ";
}
print "\n";
