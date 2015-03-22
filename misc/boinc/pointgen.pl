#!/usr/bin/perl

if($#ARGV < 1) {
  print "Usage: gener.pl #point #vars\n";
  exit;
}
$x = 0.1;
$np = @ARGV[0];
$na = @ARGV[1];
$nv = $na * 3;
$val = 0.0;
$left = 0.0;
$right = 1.0;
my @a;
for $i (1..$np) {
  my $st = "<solu><x>";
  my $ts;
  for $j (1..$nv){
      $x = rand();
      $y = $left + ($right - $left) * $x;
      $ts = sprintf("%.3f ", $y );
      $st = $st . $ts;
  }
  $ts = sprintf("</x><f>%.3f</f></solu>", $val);
  $st = $st.$ts;
  @a = (@a, $st);
  #print "$st \n";
}
foreach(@a) {
  print $_;
  print "\n";
}

open(F1,  "tmpl.xml") or die "cant open template file";
@tfile = <F1>;
close(F1) or die $!;
$i = 0;
foreach $m (@a) {
  $i ++;
  $fname = "wus/wu$i";
  open(F3, "> $fname") or die "cant open wu file\n";
  print F3 replace($m);
  close(F3);
}

sub replace {
  my $point = $_[0];
  my $str = "@tfile";
  $str =~ s/SOLUTMPL/$point/g;
  $str =~ s/NATOMTMPL/$na/g;
  $str =~ s/NVARTMPL/$nv/g;
  return $str;
}
