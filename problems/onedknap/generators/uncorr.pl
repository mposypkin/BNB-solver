#!/usr/bin/perl

# Generates uncorrelated instance with N items
# with wi random in [1, R]
# pi random in [1, R]
# and capacity c * (w1 + w2 + ... + wn)
use List::Util qw [min max];
use knapprinter;

my @p;
my @w;
my $N;
my $R;
my $a;
my $c;
my $W = 0;

my $narg = @ARGV;
($narg == 3) or die "usage genknap.pl N R c\n";
$N = $ARGV[0];
$R = $ARGV[1];
$c = $ARGV[2];
my $i;
for($i = 0; $i < $N; $i ++) {
  my $nw = max(1,int($R * rand()));
  my $np = max(1, int($R * rand()));
  $W += $nw;
  push @w, $nw;
  push @p, $np;
}
printJSON($N, $W, $c, \@w, \@p);