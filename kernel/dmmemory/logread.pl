#!/usr/bin/env perl
# Process logs produced by DM solver
my $tsteps = 0;
my $sbytes = 0;
my $rbytes = 0;
my $ttime;
while(<>) {
  if($_ =~ /(.*):(.*)/) {
    my @a = split(" ", $2);
    print "process $1: \n";
    print "\t sent parcels: $a[0]\n";
    print "\t sent bytes: $a[1]\n";
    print "\t sent subproblems: $a[2]\n";
    print "\t sent records: $a[3]\n";
    print "\t sent commands: $a[4]\n";
    print "\t received parcels: $a[5]\n";
    print "\t received bytes: $a[6]\n";
    print "\t received subproblems: $a[7]\n";
    print "\t received records: $a[8]\n";
    print "\t received commands: $a[9]\n";
    print "\t steps done: $a[10]\n";
#sent_parcels << " " << sent_subs << " " << sent_records << " " << sent_commands << " " << recv_parcels
#                << " " << recv_subs << " " << recv_records << " " << recv_commands << " " << steps_done;
    print "\n";
    $sbytes += $a[1];
    $rbytes += $a[6];
    $tsteps += $a[10];
  } 
  if ($_ =~ /time = (.*)/){
    $ttime = $1;
  }
}
  my $speed;
  if($ttime > 0) {
    $speed = $tsteps / $ttime;
  } else {
    $speed = undef;
  }
  print "Total steps: $tsteps, total bytes sent: $sbytes, total bytes received: $rbytes, total time: $ttime, processing speed: $speed\n";
