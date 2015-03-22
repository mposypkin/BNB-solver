#include <stdio.h>
#include "random.hpp"


main()
{
  
  int n = 10000;
  int m = 100;
  Gaussian gauss;
  gauss.setSeqLen(60);
  double A = -10.;
  double B = 10.;
  double h = (B - A) / m;
  int* x = (int*) calloc(m, sizeof(int));
  for(int i = 0; i < n; i ++) {
    double r = gauss.getRand();
    for(int j = 0; j < m; j ++) {
      if((A + j * h <= r) && (A + (j + 1) * h >= r)) {
        x[j] ++;
      }
    }
  }
  for(int j = 0; j < m; j ++) {
    printf("%d\t %ld\n", j - (m / 2), x[j]);
  }
  free(x);
}