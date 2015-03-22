#!/usr/bin/perl


sub paretoPairs{
  my @plist;
  foreach(@_) {
#print "@$_\n";
#print "\n";
    my $nn = $n - @{$_}[1];
    my $ww = @{$_}[2];
    my $h = 0;
    my $ad = 1;
    if($#plist == -1) {
      my @c = ($nn, $ww);
      push(@plist, \@c);
    }
    while(1){
      if($h == $#plist + 1){
	last;
      } else {
	my @b = @{$plist[$h]};
#	print "cmp $b[0], $b[1] with $nn, $ww\n";
	if(($b[0] <= $nn) && ($b[1] <= $ww)) {
	  delete $plist[$h];
	  print "deleted\n";
	} elsif(($nn <= $b[0]) && ($ww <= $b[1])) {
	  $ad = 0;
	  print "dominated\n";
	  last;
	} else {
	  $h ++;
	}
      }
    }
    if($ad) {
      print "pushed $ad\n";
      my @c = ($nn, $ww);
      push(@plist, \@c);
    } else {
      print "skipped\n";
    }
  }
  foreach(@plist) {
    print "@$_\n";
    print "\n";
  }
  print "num = $#plist\n";
}


sub pushElem {
  my $ref = $_[0];
  my $elem = $_[1];
  my $n = $#{$ref};
  my $b = 0;
  my $e = $n;
  my $v = $elem->[0];
  while($e - $b > 1){
   my $i = int(0.5 * ($b + $e));
   my $w = $ref->[$i]->[0];
   if($w >= $v) {
     $b = $i;
   } else {
     $e = $i;
   } 
  } 
  if($ref->[$b]->[0] <= $v) {
    splice @{$ref}, $b, 0, $elem;
  } elsif ($ref->[$e]->[0] <= $v) {
    splice @{$ref}, $e, 0, $elem;
  } else {
    splice @{$ref}, $e + 1, 0, $elem;
  }
}


open(DAT, $ARGV[0]) || die "can't open $ARGV[0]";
while(<DAT>) {
  if(/<n>(.*)<\/n>/){
    $n=$1;
  } 
  if(/<w>(.*)<\/w>/){
    @w = split(" ", $1);
  }
  if(/<p>(.*)<\/p>/){
    @p = split(" ", $1);
  }
  if(/<c>(.*)<\/c>/){
    $c=$1;
  } 
}
close(DAT);
$s = 0;
foreach(@w) {
  $s += $_;
}

$imav = $n * $c;
@a = ($n * $c, 0, $c, $s);
@list = (\@a);
#while(<STDIN>) {
while($#list < 100000)  {
#  @list = sort {$b->[0] <=> $a->[0]} @list;
  $mav = $list[0]->[0];
  $miv = $list[$#list]->[0];
  if(20 * $mav < $imav){
    last;
  }
  $rat = $imav / $mav;
  print "$#list: $mav $miv: $rat\n";
#  foreach(@list) {
#    print "@$_\n";
#  }
  my @b = @{$list[0]};
  $m = $b[1];
  $cc = $b[2];
  $ss = $b[3];
  $mn = $m + 1;
  $wn = $w[$m];
  $sn = $s - $wn;
  $flag = 0;
  foreach(0..1){
    $cn = $cc - $_ * $wn;
    if(($mn < $n) && ($cn >= 0) && ($sn > $cn)) {
      my @an = ();
      push(@an, ($n - $mn) * $cn);
      push(@an, $mn);
      push(@an, $cn);
      push(@an, $sn);
      for($i = 4; $i <= $#b; $i ++){
	push(@an, $b[$i]);
      }
      push @an, $_;
#push(@list, \@an);
      pushElem(\@list, \@an);
#      print "added\n";
      $flag ++;
    }
  }
  if($flag > 0) {
    shift @list;
  } else {
    print "Breaking\n";
    last;
  }
  
}
  paretoPairs(@list);

