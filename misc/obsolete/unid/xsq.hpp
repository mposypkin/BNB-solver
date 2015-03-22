#ifndef __XSQ_HPP__
#define __XSQ_HPP__

#include <problems/go/lipzobjective.hpp>

class Xsq : public LipzObjective<double> {
  public:
    
    Xsq ()
    {
      setDim(1);
    }
    
    double func(double* x)
    {
      double v = x[0] * x[0];
      return v;
    }
        
    void grad(double* x, double* g)
    {
        g[0] = 2 * x[0];
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      return BNBMAX(BNBABS(2 * a[0]), BNBABS(2 * b[0]));      
    }
   
    double getGradLipzConstant(double* a, double* b)
    {
      return 2.;
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      return 0.;
    }
    
};
#endif
