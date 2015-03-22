#ifndef __C1_HPP__
#define __C1_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class C1 :  public LipzObjective<double> {
  
  public:
    
  C1()
  {
    Objective<double>::setDim(2);
  }
    
  double func(const double* x)
  {
    double v;
    v = - BNBSQR(x[0] - 5.) - BNBSQR(x[1] - 5.) + 100.;
    return v;
  }
        
  void grad(const double* x, double* g)
  {
    g[0] = -2. * (x[0] - 5.);
    g[1] = -2. * (x[1] - 5.);
  }
    
  double getFuncLipzConstant(double* a, double* b)
  {
    double v;
    v = BNBMAX(BNBSQR(a[0] - 5.), BNBSQR(b[0] - 5.));
    v += BNBMAX(BNBSQR(a[1] - 5.), BNBSQR(b[1] - 5.));
    return 2. * sqrt(v);
  }
    
  double getGradLipzConstant(double* a, double* b)
  {
    return 2.;
//     return 0.25;
  }
    /*
  double getGradCompLipzConstant(double* a, double* b, int i)
  {
  return 0.;
}
    */
};


#endif

