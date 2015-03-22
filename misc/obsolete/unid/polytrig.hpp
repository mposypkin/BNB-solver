#ifndef __POLYTRIG_HPP_
#define __POLYTRIG_HPP_

#include <math.h>
#include <problems/go/lipzobjective.hpp>
#include <util/common/interval.hpp>

class Polytrig : public LipzObjective<double> {
  public:
    
    Polytrig ()
    {
      setDim(1);
      mA = 18.;
    }
    
    double func(double* x)
    {
      double y = x[0];
      double v = y*y - cos(mA * y);
      return v;
    }
        
    void grad(double* x, double* g)
    {
      double y = x[0];
      double v = 2 * y + mA * sin(mA * y);
      g[0] = v;
    }
    
    int hessn(double* x, double* H)
    {
      double y = x[0];
      H[0] = 2 + mA * mA * cos(mA * y);
      return 1;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double l, u, cl, cu;
      BNBInterval<double>::sin(mA * a[0], mA * b[0], &l, &u);
      BNBInterval<double>::sum(2*a[0], 2*b[0], mA * l, mA * u, &l, &u);
      return BNBMAX(BNBABS(l), BNBABS(u));      
    }
   
    double getGradLipzConstant(double* a, double* b)
    {
      double l, u, q = mA * mA;
      BNBInterval<double>::cos(mA * a[0] , mA * b[0], &l, &u);
      return BNBMAX(BNBABS(q * l + 2), BNBABS(q * u + 2));      
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      return getGradLipzConstant(a, b);
    }
    
    double getHessLipzConstant(double* a, double* b)
    {
      double l, u, p = mA * mA * mA;
      BNBInterval<double>::sin(mA * a[0], mA * b[0], &l, &u);
      return p * BNBMAX(BNBABS(l), BNBABS(u));      
    }
    
  private:
    
    double mA;
};

#endif
