#include <stdio.h>
#include "infini.hpp"

main()
{
  unsigned int a = 127, b = 10128;
  InfiniInt ii, jj, kk, zero;
  
  ii = a;
  jj.fromString("10128");
  kk = ii + jj ;
  ++ kk;
  printf("%d = %s\n", a + b + 1, kk.toString().c_str());
  printf("%s %s %s\n", ii.toString().c_str(), (ii < kk) ? "<" : ">", kk.toString().c_str());
  printf("%s %s %s\n", ii.toString().c_str(), (ii <= kk) ? "<=" : ">", kk.toString().c_str());
  printf("%s %s %s\n", ii.toString().c_str(), (ii <= ii) ? "<=" : ">", ii.toString().c_str());
  printf("%s %s %s\n", jj.toString().c_str(), (jj <= ii) ? "<=" : ">", ii.toString().c_str());
  printf("%s %s %s\n", zero.toString().c_str(), (zero == 0) ? "=" : "!=", "0");
  zero = 0;
  printf("%s %s %s\n", zero.toString().c_str(), (zero == 0) ? "=" : "!=", "0");
}
