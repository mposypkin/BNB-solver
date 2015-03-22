#!/usr/bin/perl
use knapprinter;

my @p;
my @w;
my $n;
my $c;
my $ampl;

sub printKnap {
  print "{\n";
  print "\"N\" : $n,\n";
  print "\"C\" : $c,\n";
  my $i;
  print "\"P\" : [";
  for($i = 0; $i < $n; $i ++) {
    if($i > 0) {
      print ", ";
    }
    print "$p[$i]";
  }
  print "],\n";
  print "\"W\" : [";
  for($i = 0; $i < $n; $i ++) {
    if($i > 0) {
      print ", ";
    }
    print "$w[$i]";
  }
  print "]\n";
  print "}\n";
}

my $narg = @ARGV;
($narg == 3) or die "usage simpgen.pl N A C\n";
$n = $ARGV[0];
$ampl = $ARGV[1];
$c = $ARGV[2];
my $i;
for($i = 0; $i < $n; $i ++) {
  my $np = 1 + int($ampl * rand());
  push @p, $np;
}

for($i = 0; $i < $n; $i ++) {
  my $nw = 1 + int($ampl * rand());
  push @w, $nw;
}

printKnap();
