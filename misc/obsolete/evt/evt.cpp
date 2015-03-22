#include <stdio.h>
#include <stdlib.h>
#include <problems/multigo/multigo.hpp>
#include "ccc.hpp"
#include "evt.hpp"
#include "evtm.hpp"

void pareto(int N, int m, int n, double* a, double* b, MultiFunction& cf, MGOFactory::Solution &sol)
{
  for(int i = 0; i < N; i ++) {
    MGOFactory::Point point(n, m);
    for(int i = 0; i < n; i ++) {
      point.mParams[i] = a[i] + ((b[i] - a[i]) * rand()) / RAND_MAX;
    }
    cf.func(point.mParams, point.mCriteria);
    sol.pushPoint(point);
  }
}

main()
{
  int N = 100;
  int n;
  int m;
  
#if 0  
  double a[2] = {-0.5, -0.5};
  double b[2] = {0.5, 0.5};
  m = 2;
  n = 2;
  Ccc cf;
#endif

#if 1 
  double a[1] = {0.};
  double b[1] = {2.};
  m = 2;
  n = 1;
  Evt cf;
#endif

#if 0 
  double a[2] = {0., 0.};
  double b[2] = {2., 2.};
  m = 2;
  n = 2;
  Evtm cf;
#endif
  MGOFactory::Solution sol;
  pareto(N, m, n, a, b, cf, sol);
  printf("%s\n", sol.toString().c_str());
}
