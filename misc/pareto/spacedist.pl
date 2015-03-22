#!/usr/bin/perl

# Script measures the uniformity of the pareto-front approximation
# distribution

sub dist {
  my ($n, $a, $b) = @_;
  my $d = 0;
  my $j = 0;
  for($j = 0; $j < $n; $j ++) {
    my $r = ($a->[$j] - $b->[$j]);
    $d += $r * $r;
  }
  return sqrt($d);
}

sub ave {
  my $s = 0;
  foreach $e (@_) {
    $s += $e;
  }
  return $s / @_;
}

sub metr {
  my $n = @_;
  my $av = ave(@_);
  my $s = 0;
  foreach $e(@_) {
    my $r = $av - $e;
    $s += $r * $r;
  }
  $s /= $n;
  $s = sqrt($s);
  return $s / $av;
}


my @x;
my $j = 0;
my $n = 0;
while(<>) {
  my @a = split;
  my $l = @a;
  if($n == 0) {
    $n = $l;
  } elsif ($n != $l) {
    die "all tuples must have same length!\n";
  }
  $x[$j] = [@a];
  $j ++;
}

my @D;
my $e1, $e2;
foreach $e1 (@x) {
  my $mind = -1;
  foreach $e2 (@x) {
    unless($e1 == $e2) {
      my $d = dist($n, $e1, $e2);
      if($mind == -1) {
        $mind = $d;
      } elsif($d < $mind) {
	$mind = $d;
      }
    }
  }
  @D = (@D, $mind);
}
print "D: @D\n";
my $av = ave(@D);
print "ave D: $av\n";
my $metr = metr(@D);
print "metr D: $metr\n";
