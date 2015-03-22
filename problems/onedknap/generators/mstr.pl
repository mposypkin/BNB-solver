#!/usr/bin/perl

# Generated multiple strongly correlated instances
# wi are random in [1,R]
# if wi is divisable by d pi = wi + k1
# otherwise pi = wi + k2
# and capacity c * (w1 + w2 + ... + wn)

use List::Util qw [min max];
use knapprinter;

my $narg = @ARGV;
($narg == 6) or die "usage genknap.pl N R k1 k2 d c\n";
$N = $ARGV[0];
$R = $ARGV[1];
$k1 = $ARGV[2];
$k2 = $ARGV[3];
$d = $ARGV[4];
$c = $ARGV[5];
my $i;
my @p;
my @w;
my $W = 0;
for($i = 0; $i < $N; $i ++) {
  my $nw = max(1,int($R * rand()));
  my $np;
  if($nw % $d) {
    $np = $nw + $k1;
  } else {
    $np = $nw + $k2;
  }
  $np = max(1, $np);
  $W += $nw;
  push @w, $nw;
  push @p, $np;
}

printJSON($N, $W, $c, \@w, \@p);
