#!/usr/bin/perl

# This script takes an output of the multiobjective optimization
# algorithm in XML format and outputs its results in parameter or
# function value pairs

$tag = "";
$ad = "";

if($#ARGV >= 0) {
  $tag = @ARGV[0];
  if($#ARGV >= 1) {
    $ad = @ARGV[1];
  }
} else {
  $tag = "f";
}
while(<STDIN>) {
  if(/<$tag>(.*)<\/$tag>/){
    print "$1 $ad";
    print "\n";
  }
}
