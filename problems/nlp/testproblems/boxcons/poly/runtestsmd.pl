#!/usr/bin/perl

# Comparative testing of various box reduction techniques
# n-dimensional case

sub run_test {
  my $d = $_[0];
  my $name = $_[1];
  print "Running $name with simple box reduction\n";
  my $rstr = "./runpolybc.exe $d 10 \"$name\" 1 0";
  #my $ts = (times)[0];
  system($rstr);
  #my $te = (times)[0];
  #my $t = $te - $ts;
  #printf "%.2f seconds elapsed\n", $t;
  print "Running $name with advanced box reduction\n";
  my $rstr = "./runpolybc.exe $d 10 \"$name\" 1 1";
  print "$rstr\n";
  system($rstr);
}


print "Running m-d benchmarks\n";

# Simplest saddle test
 run_test(4, "x^2 - y^2 - z^2 + t^2");

# Wood's function
 run_test(4, "100 x1^4 + 90 x3^4 - 200 x1^2 x2 - 180 x3^2 x4  + x1^2 + 110.1 x2^2 + x3^2 + 100.1 x4^2 + 19.8 x2 x4 - 2 x1 - 40 x2 - 2 x3 - 40 x4 + 42");

# Newmier's  3 function 3 variable
 run_test(3, "x1^2 + x2^2 + x3^2  - x2 x1 - x3 x2  - 2 x1 - 2 x2 - 2 x3  + 3");

# Newmier's  3 function 4 variable
 run_test(4, "x1^2 + x2^2 + x3^2 + x4^2 - x2 x1 - x3 x2 - x4 x3 - 2 x1 - 2 x2 - 2 x3 - 2 x4 + 4");
