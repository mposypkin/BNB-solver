package knapprinter;

use base 'Exporter';

our @EXPORT = qw(printJSON);

sub printJSON  {
  my ($N, $W, $c, $rw, $rp) = @_;
  print "{\n";
  print "\"N\" : $N,\n";
  my $C = int($c * $W);
  print "\"C\" : $C,\n";
  my $i;
  print "\"P\" : [";
  for($i = 0; $i < $N; $i ++) {
    if($i > 0) {
      print ", ";
    }
    print "$rp->[$i]";
  }
  print "],\n";
  print "\"W\" : [";
  for($i = 0; $i < $N; $i ++) {
    if($i > 0) {
      print ", ";
    }
    print "$rw->[$i]";
  }
  print "]\n";
  print "}\n";
}