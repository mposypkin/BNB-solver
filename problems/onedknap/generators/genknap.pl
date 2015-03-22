#!/usr/bin/perl

# Generates instance with N items
# with wi random in [1, R]
# pi random in [max(1, wi + a * R - b * R) : wi + a * R + b * R]
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
($narg == 5) or die "usage genknap.pl N R a b c\n";
$N = $ARGV[0];
$R = $ARGV[1];
$a = $ARGV[2];
$b = $ARGV[3];
$c = $ARGV[4];
my $i;
for($i = 0; $i < $N; $i ++) {
  my $nw = max(1,int($R * rand()));
  my $pb = $nw + int($a * $R);
  my $pr = $pb + int(2 * $b * $R * (0.5 - rand()));
  my $np = max(1, $pr);
  $W += $nw;
  push @w, $nw;
  push @p, $np;
}
printJSON($N, $W, $c, \@w, \@p);