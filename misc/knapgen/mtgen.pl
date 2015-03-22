#!/usr/bin/perl

# Simple knapsack generator that generates instances
# of n items
# with weights uniformly distributed in [1, R] and
# profit pi uniformly distributed in [wi + S - D, wi + S + D]
# and capacity C

(scalar @ARGV == 5) or die "usage: mtgen.pl n R S D C";

$n = $ARGV[0];
$R = $ARGV[1];
$S = $ARGV[2];
$D = $ARGV[3];
$C = $ARGV[4];

print "$C\n";

for($i = 0; $i < $n; $i ++) {
    my $w = 1 + int(rand($R - 1));
    my $q = int(rand(2 * $D)) - $D;
    my $p = $w + $S + $q;
    print "$p $w \n";
}