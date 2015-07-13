#!/usr/bin/perl

# Comparative testing of various box reduction techniques
# 2-dimensional case

sub run_test {
  my $name = $_[0];
  print "Running $name with simple box reduction\n";
  my $rstr = "./runpolybc.exe 2 10 \"$name\" 1 0";
  #my $ts = (times)[0];
  system($rstr);
  #my $te = (times)[0];
  #my $t = $te - $ts;
  #printf "%.2f seconds elapsed\n", $t;
  print "Running $name with advanced box reduction\n";
  my $rstr = "./runpolybc.exe 2 10 \"$name\" 1 1";
  system($rstr);
}


print "Running 2d benchmarks\n";

# Simplest saddle test
run_test("x^2 - y^2");

# Alluffi-Pentini test
run_test("0.25 x^4 - 0.5 x^2 + 0.1 x + 0.5 y^2"); 

# Three hump Camel-back function
run_test("0.16666 x^6 - 1.05 x^4 + 2 x^2 + x y + y^2");

# Six hump Camel-back function
run_test("0.33333 x^6 - 2.1 x^4 + 4 x^2 + x y + 4 y^4 - 4 y^2");

# Newmier problem 1
run_test("x^2 + y^2 - x y - 2 x - 2 y + 2");

# Rosenbrok function
run_test("100 x^4 - 200 x^2 y + x^2 + 100 y^2 - 2 x + 1");