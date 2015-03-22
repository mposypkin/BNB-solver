#ifndef __C1_HPP__
#define __C1_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class C1 :  public LipzObjective<double> {
  
  public:
    
  C1()
  {
    Objective<double>::setDim(2);
    Objective<double>::setKind(Objective<double>::EQCONSTRAINT);
  }
    
  double func(const double* x)
  {
    double v;
    v = x[1] - x[0] * x[0];
    return v;
  }
        
  void grad(const double* x, double* g)
  {
    g[0] = - 2. * x[0];
    g[1] = 1.;
  }
    
  double getFuncLipzConstant(double* a, double* b)
  {
    double v = 1.;
    v += 4. * BNBMAX(a[0] * a[0], b[0] * b[0]);    
    return sqrt(v);
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

