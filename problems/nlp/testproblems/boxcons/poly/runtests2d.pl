#!/usr/bin/perl

# Comparative testing of various box reduction techniques
# 2-dimensional case

sub run_test {
  my $name = $_[0];
  my $rec = $_[1];
  print "Running $name with simple box reduction, preset incumbent = $rec\n";
  my $rstr = "./runpolybc.exe 2 20 \"$name\" 1 0 $rec";
  #my $ts = (times)[0];
  system($rstr);
  #my $te = (times)[0];
  #my $t = $te - $ts;
  #printf "%.2f seconds elapsed\n", $t;
  print "Running $name with advanced box reduction, preset incumbent = $rec\n";
  my $rstr = "./runpolybc.exe 2 20 \"$name\" 1 1 $rec";
  system($rstr);
}


print "Running 2d benchmarks\n";

# Simple hyperboloid test

run_test("x y", 10000);
run_test("x y", -400);

# Simplest square difference test
run_test("x^2 - y^2", 10000);
run_test("x^2 - y^2", -400);

# Alluffi-Pentini test
run_test("0.25 x^4 - 0.5 x^2 + 0.1 x + 0.5 y^2", 10000); 
run_test("0.25 x^4 - 0.5 x^2 + 0.1 x + 0.5 y^2", -0.3523); 

# Three hump Camel-back function
run_test("0.16666 x^6 - 1.05 x^4 + 2 x^2 + x y + y^2", 10000);
run_test("0.16666 x^6 - 1.05 x^4 + 2 x^2 + x y + y^2", 0);

# Six hump Camel-back function
run_test("0.33333 x^6 - 2.1 x^4 + 4 x^2 + x y + 4 y^4 - 4 y^2", 10000);
run_test("0.33333 x^6 - 2.1 x^4 + 4 x^2 + x y + 4 y^4 - 4 y^2", -1.0316);

# Newmier problem 1
run_test("x^2 + y^2 - x y - 2 x - 2 y + 2", 10000);
run_test("x^2 + y^2 - x y - 2 x - 2 y + 2", -2);

# Rosenbrok function
run_test("100 x^4 - 200 x^2 y + x^2 + 100 y^2 - 2 x + 1", 10000);
run_test("100 x^4 - 200 x^2 y + x^2 + 100 y^2 - 2 x + 1", 1);

# Booth  function
run_test("5 x^2+8 x y-34 x+5 y^2-38 y+74", 10000);
run_test("5 x^2+8 x y-34 x+5 y^2-38 y+74", 0);

#Goldstein-Prices function
run_test("144 x^8-288 x^7 y-768 x^7-648 x^6 y^2+1344 x^6 y+952 x^6+1224 x^5 y^3+2592 x^5 y^2-168 x^5 y+1344 x^5+1305 x^4 y^4-4080 x^4 y^3-5370 x^4 y^2-7680 x^4 y-2454 x^4-1836 x^3 y^5-3480 x^3 y^4+1240 x^3 y^3+9840 x^3 y^2+5784 x^3 y-1072 x^3-1458 x^2 y^6+3672 x^2 y^5+8730 x^2 y^4+5040 x^2 y^3+7776 x^2 y^2+7344 x^2 y+1260 x^2+972 x y^7+1944 x y^6-1188 x y^5-11880 x y^4-23616 x y^3-19296 x y^2-4680 x y+720 x+729 y^8-648 y^7-4428 y^6+1944 y^5+14346 y^4+12288 y^3+3060 y^2+720 y+600", 10000);
run_test("144 x^8-288 x^7 y-768 x^7-648 x^6 y^2+1344 x^6 y+952 x^6+1224 x^5 y^3+2592 x^5 y^2-168 x^5 y+1344 x^5+1305 x^4 y^4-4080 x^4 y^3-5370 x^4 y^2-7680 x^4 y-2454 x^4-1836 x^3 y^5-3480 x^3 y^4+1240 x^3 y^3+9840 x^3 y^2+5784 x^3 y-1072 x^3-1458 x^2 y^6+3672 x^2 y^5+8730 x^2 y^4+5040 x^2 y^3+7776 x^2 y^2+7344 x^2 y+1260 x^2+972 x y^7+1944 x y^6-1188 x y^5-11880 x y^4-23616 x y^3-19296 x y^2-4680 x y+720 x+729 y^8-648 y^7-4428 y^6+1944 y^5+14346 y^4+12288 y^3+3060 y^2+720 y+600", 3);

# Dekker & Aarts problem
#run_test("0.00001 x^8 + 0.00004 x^6 y^2 + 0.00006 x^4 y^4 - x^4 + 100000 x^2 + 0.00004 x^2 y^6 -2 x^2 y^2 + 0.00001 y^8 - y^4 + y^2", 10000);
#run_test("0.00001 x^8 + 0.00004 x^6 y^2 + 0.00006 x^4 y^4 - x^4 + 100000 x^2 + 0.00004 x^2 y^6 -2 x^2 y^2 + 0.00001 y^8 - y^4 + y^2", -24776.5);

# Modified Rosenbrock function
run_test("40.96 y^4 - 81.92 y^3 + 153.76 y^2 - 12.8 y^2 x - 200 y x^2 + 12.8 y x - 12.8 y + 1 - 2 x + 100 x^4 + x^2", 10000);
run_test("40.96 y^4 - 81.92 y^3 + 153.76 y^2 - 12.8 y^2 x - 200 y x^2 + 12.8 y x - 12.8 y + 1 - 2 x + 100 x^4 + x^2", 0);
