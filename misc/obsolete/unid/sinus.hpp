#ifndef __SINUS_HPP__
#define __SINUS_HPP__

#include <math.h>
#include <problems/go/lipzobjective.hpp>
#include <util/common/interval.hpp>

class Sinus : public LipzObjective<double> {
  public:
    
    Sinus ()
    {
      setDim(1);
    }
    
    double func(double* x)
    {
      double y = x[0];
      double v = sin(y);
      return v;
    }
        
    void grad(double* x, double* g)
    {
      double y = x[0];
      double v = cos(y);
      g[0] = v;
    }
    
    int hessn(double* x, double* H)
    {
      double y = x[0];
      H[0] = - sin(y);
      return 1;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double l, u;
      BNBInterval<double>::cos(a[0], b[0], &l, &u);
      return BNBMAX(BNBABS(l), BNBABS(u));      
    }
   
    double getGradLipzConstant(double* a, double* b)
    {
      double l, u;
      BNBInterval<double>::sin(a[0], b[0], &l, &u);
      return BNBMAX(BNBABS(l), BNBABS(u));      
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      return getGradLipzConstant(a, b);
    }
    
    double getHessLipzConstant(double* a, double* b)
    {
      double l, u;
      BNBInterval<double>::cos(a[0], b[0], &l, &u);
      return BNBMAX(BNBABS(l), BNBABS(u));      
    }

};


#endif
