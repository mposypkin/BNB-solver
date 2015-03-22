#ifndef __TRIG_HPP__
#define __TRIG_HPP__

#include <math.h>
#include <problems/go/lipzobjective.hpp>
#include <util/common/interval.hpp>

class Trig : public LipzObjective<double> {
  public:
    
    Trig ()
    {
      setDim(1);
    }
    
    double func(double* x)
    {
      double y = x[0];
      double v = cos(y)  + y * sin(y);
      return v;
    }
        
    void grad(double* x, double* g)
    {
      double y = x[0];
      double v = y * cos(y);
      g[0] = v;
    }
    
    int hessn(double* x, double* H)
    {
      double y = x[0];
      H[0] = cos(y) - y * sin(y);
      return 1;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double l, u;
      BNBInterval<double>::cos(a[0], b[0], &l, &u);
      BNBInterval<double>::mult(a[0], b[0], l, u, &l, &u);
      return BNBMAX(BNBABS(l), BNBABS(u));      
    }
   
    double getGradLipzConstant(double* a, double* b)
    {
      double l, u, cl, cu, sl, su;
      BNBInterval<double>::cos(a[0], b[0], &cl, &cu);
      BNBInterval<double>::sin(a[0], b[0], &sl, &su);
      BNBInterval<double>::mult(a[0], b[0], sl, su, &l, &u);
      BNBInterval<double>::diff(cl, cu, l, u, &l, &u);
      return BNBMAX(BNBABS(l), BNBABS(u));      
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      return getGradLipzConstant(a, b);
    }
    
    double getHessLipzConstant(double* a, double* b)
    {
      double l, u, cl, cu, sl, su;
      BNBInterval<double>::cos(a[0], b[0], &cl, &cu);
      BNBInterval<double>::sin(a[0], b[0], &sl, &su);
      BNBInterval<double>::mult(a[0], b[0], cl, cu, &l, &u);
      BNBInterval<double>::sum(sl, su, l, u, &l, &u);
      return BNBMAX(BNBABS(l), BNBABS(u));      
    }

};


#endif
