#!/usr/bin/perl
sub morse {
  my ($rho, $r) = @_;
  my $e = exp($rho * (1 - $r));
  my $rv = $e * ($e - 2);
  return $rv;
}

sub energy {
  my ($na, @x) = @_;
  my $i, $j;
  my $rv = 0;
  for($i = 0; $i < $na; $i ++) {
    my ($x1, $x2, $x3) = @x[3 * $i, 3 * $i + 1, 3 * $i + 2];
    for($j = $i + 1; $j < $na; $j ++) {
      my ($y1, $y2, $y3) = @x[3 * $j, 3 * $j + 1, 3 * $j + 2];
      my $r = sqrt(($y1 - $x1) ** 2 + ($y2 - $x2) ** 2 + ($y3 - $x3) ** 2);
      my $v = morse(14, $r);
      $rv += $v;
    }
  }
  return $rv;
}



if($#ARGV != 2) {
  die "Usage: gener.pl #point #natom boxsize\n";
}
$d = 3;
$x;
$np = @ARGV[0];
$na = @ARGV[1];
$boxside=@ARGV[2];
$val = 0.0;
$side = 1;
while($side ** 3 < $na) {
  $side ++;
}
$scale = $boxside / ($side - 1);
$cels = $side ** 3;
for($I = 0; $I < $np; $I ++) {
  my $n = 0;
  my %atoms = ();
  while($n < $na) {
    my $x = int(rand() * $cels);
    unless(defined($atoms{$x})) {
      $atoms{$x} = 1;
      $n ++;
    }
  }
  my @a = ();
  foreach(keys(%atoms)) {
    my $i;
    my $u = $_;
    for($i = 0; $i < $d; $i ++) {
      my $v = int($u / $side);
      my $r = $u - $v * $side;
      $r *= $scale;
      @a = (@a, $r);
      $u = $v;
    }
  }

  $val = energy($na, @a);
  print "<solu><x>@a</x><f>$val</f></solu>\n";
}
