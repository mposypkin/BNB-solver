#!/usr/bin/perl

sub printRecord {
  my $ref = $_[0];
  my $sft = $_[1];
  for (keys %{$ref}) {
    print "$sft $_ => ${$ref}{$_}\n";
  }
#  if(defined(${$ref}{r})) {
#    printRecord(${$ref}{r}, $sft."  ");
#  }
}

sub pushElem {
  my $refa = $_[0];
  my $refb = $_[1];
  my $bn = $refb->{n};
  my $bc = $refb->{c};
  my $flag = 0;
  my $n = $#{$refa};
  my $i;
  for($i = 0; $i <= $n; $i ++){
    my $R = $refa->[$i];
    my $an = $R->{n};
    my $ac = $R->{c};
    if(($bn == $an) && ($bc == $ac)) {
      $flag = 1;
      last;
    } elsif (($bn >= $an) && ($bc >= $ac)) {
      $refb->{r} = \%R;
      $refa->[$i] = $refb;
      $flag = 1;
      last;
    } elsif (($bn <= $an) && ($bc <= $ac)) {
      my $NR = $R;
      while(1) {
	my $ref = $NR->{r};
	if(!defined($ref)) {
	  $NR->{r} = $refb;
	  last;
	} else {
	  my $NNR = $ref;
	  if(($NNR->{c} == $refb->{c})&&($NNR->{n} == $refb->{n})) {
	    last;
	  } elsif($NNR->{nc} < $refb->{nc}) {
	    $refb->{r} = $ref;
	    $NR->{r} = $refb;
	    last;
	  } else {
	    $NR = $NNR;
	  }
	}
      }
      $flag = 1;
      last;
    }
  }
  if(!$flag) {
    push(@{$refa}, \%{$refb});
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

$nc = $n * $c;

%a = (nc => $nc, n => $n, c => $c, s => $s, a => "", r => undef);
@list = (\%a);
my $iter = 0;
while($#list >= 0)  {
  $iter ++;
#@list = sort{$b->{nc} <=> $a->{nc}} @list;
  my $maxnc = $list[0]->{nc};
  my $ratio = $nc / $maxnc;
  print "iter $iter, $#list: $ratio, $nc, $maxnc\n";
  if($ratio >= 20) {
    last;
  }
  my %b = %{$list[0]};
  if(!defined($b{r})){
    shift @list;
  } else {
    $list[0] = $b{r};
    $b{r} = undef;
  }
  $m = $n - $b{n};
  $cc = $b{c};
  $ss = $b{s};
  $mn = $m + 1;
  $wn = $w[$m];
  $sn = $s - $wn;
  $flag = 0;
  foreach(0..1){
    $cn = $cc - $_ * $wn;
    if(($mn < $n) && ($cn >= 0) && ($sn > $cn)) {
      my $str;
      if($_) {
	$str = $b{a}."1";
      } else {
	$str = $b{a}."0";
      }
      my %an =(
	  nc => ($n - $mn) * $cn,
	 n => $n - $mn,
	 c => $cn,
	 s => $sn,
	 a => $str,
	 r => undef
	   );
#push(@list, \%an);
      pushElem(\@list, \%an);
#      print "added\n";
      $flag ++;
    }
  }
}
foreach(@list) {
  printRecord($_);
  print "----------\n";
}
