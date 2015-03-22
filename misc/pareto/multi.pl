#!/usr/bin/perl

# This script evaluates the domination between two set of points
# Copyright by Mikhail Posypkin

use Data::Dumper;

my $ftable;

sub printarr {
  my $arr = $_[0];
  foreach $i (@$arr) {
    foreach $j (@$i) {
      print "$j ";
    }
    print "\n";
  }
}

sub fillarr{
  my ($name, $arr) = @_;
  open(FF, $name) || die "can't open $name file\n";
  my $j = 0;
  while(<FF>) {
    if(/<f>(.*)<\/f>/){
      my @a = split(/ /, $1);
      $arr->[$j] = [@a];
      $j ++;
    }
  }
  $ftable{$arr} = $name;
}

sub printdom {
  my ($n, $ea, $eb) = @_;
  my $j;
  print "DOMIN $n\n";
  for($j = 0; $j < $n; $j ++) {
    print "D : $ea->[$j]  >=  $eb->[$j] \n";
  }
}

sub compare {
  my($a, $b) = @_;
  my $la = @$a;
  my $lb = @$b;
  my $dim = @{$a->[0]};

  print "$la --vs-- $lb of $dim\n";
  my $ea;
  my $ndom = 0;
  foreach $ea (@$a) {
    my $dom = 0;
    my $eb;
    my $j;
    foreach $eb (@$b) {
      for($j = 0; $j < $dim; $j ++) {
	if(($eb->[$j] <= $ea->[$j])) {
	  if($j == ($dim - 1)) {
	    $dom = 1;
	    printdom($dim, $ea, $eb);
	    last;
	  } 
	} else {
	  last;
	}
      }
      if($dom == 1) {
	last;
      }
    }
    $ndom += $dom;
  }
  my $pdom = $ndom / $la;
  print "$ndom ($pdom) elements dominated in the array $ftable{$a} by $ftable{$b}\n";
}

my @arra, @arrb;
($#ARGV == 1) || die "Give two files to compare\n";
fillarr($ARGV[0], \@arra);
fillarr($ARGV[1], \@arrb);
compare(\@arra, \@arrb);
compare(\@arrb, \@arra);
