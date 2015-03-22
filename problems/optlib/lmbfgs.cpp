#include "lmbfgs.hpp"

Objective<double>* LMBFGS::mObjective = NULL;

extern "C" double lbfgsEvaluate(void *instance, const double *x, double *g, const int n,const lbfgsfloatval_t step)
{
  double v;
//   v = LMBFGS::mObj->gradAndFunc(x, g);  
  v = LMBFGS::mObjective->func(x);
  LMBFGS::mObjective->grad(x, g);
  return v;
}

