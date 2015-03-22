#!/usr/bin/perl
die "files not given: template points" unless @ARGV;
open(F1,  $ARGV[0]) or die "cant open template file";
@tfile = <F1>;
close(F1) or die $!;
open(F2,  $ARGV[1]) or die "cant open point file";
@pfile = <F2>;
close(F2) or die $!;
$i = 0;
foreach $m (@pfile) {
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
  return $str;
}


