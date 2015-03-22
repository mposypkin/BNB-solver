#!/usr/bin/perl 

use File::Copy;
#replaces a string with another string in all files

my @arr;
if($#ARGV == 1) {
  $s1 = $ARGV[0];
  $s2 = $ARGV[1];
} else {
  die "use: replace.pl str1 str2\n";
}
while(defined($fn = <*>)) {
    print "$fn\n";
    open(F, $fn);
    open(F1, ">".$fn."tmp");
    while(<F>) {
      s/$s1/$s2/g;
      print F1 $_;
    }
    close(F1);
    close(F);
    copy ($fn."tmp", $fn);
    unlink($fn."tmp");
}
