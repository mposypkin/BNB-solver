#!/usr/bin/perl
my $nsteps = 0;
while(<>) {
  $nargs = 4;
  %actions = (
      0 => "dummy",
      1 => "send_sub",
      2 => "send_rec",
      3 => "send_sub_and_rec",
      4 => "send_command",
      5 => "recv",
      6 => "sstrat",
      7 => "set_heur",
      8 => "set_bm",
      9 => "solve",
      10 => "exit"
      );
  %events = (
     -1 => "error",
      0 => "dummy",
      1 => "start",
      2 => "done",
      3 => "sent",
      4 => "nothsent",
      5 => "data_arrived",
      6 => "command_arrived",
      7 => "sstrat",
      8 => "heur_set",
      9 => "bm_set"
      );
  @a = split;
  $s = "";
  $s .= "\[$a[0]\] $a[1]: ";
  $s .= "action ";
  $s .= "$actions{$a[2]}";
  $s .= "[";
  for($i = 3; $i < $nargs + 3; $i ++) {
    $s .= "$a[$i]";
    if($i != $nargs + 2) {
      $s .= ",";
    } else {
      $s .= "]";
    }
  }
  $s .= " | event ";
  $s .= "$events{$a[$i]}";
  if($events{$a[$i]} eq "done") {
    $nsteps += $a[$i + 1];
  }
  $s .= "[";
  $iold = ++ $i;
  for(; $i < $nargs + $iold; $i ++) {
    $s .= "$a[$i]";
    if($i != $nargs + $iold - 1) {
      $s .= ",";
    } else {
      $s .= "]";
    }
  }
  $s .= " | ";
  $s .= " nsub = $a[$i ++],";
  $s .= " recup = $a[$i]";
  print "$s\n";
  print "Total number of steps is $nsteps\n";
}
