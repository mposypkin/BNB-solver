#ifndef __F_HPP__
#define __F_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class F :  public LipzObjective<double> {
  
  public:
    
  F()
  {
    Objective<double>::setDim(2);
  }
    
  double func(const double* x)
  {
    double v;
    v = x[0] * x[0] + BNBSQR(x[1] - 1);
    return v;
  }
        
  void grad(const double* x, double* g)
  {
    g[0] = 2 * x[0];
    g[1] = 2 * (x[1] - 1.);
  }
    
  double getFuncLipzConstant(double* a, double* b)
  {
    double v = 0.;
    v += BNBMAX(a[0] * a[0], b[0] * b[0]);
    v += BNBMAX((a[1] - 1.) * (a[1] - 1.), (b[1] - 1.) * (b[1] - 1.));
    return 2. * sqrt(v);
  }
    
  double getGradLipzConstant(double* a, double* b)
  {
    return 2.;
  }
    /*
  double getGradCompLipzConstant(double* a, double* b, int i)
  {
    return 0.;
  }
*/
};

#endif

