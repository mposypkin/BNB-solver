#!/usr/bin/perl

while(<STDIN>) {
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

print "$n\n";
for($i = 0; $i < $n; $i ++) {
  print "$w[$i] $p[$i]\n";
}
print "$c\n";
