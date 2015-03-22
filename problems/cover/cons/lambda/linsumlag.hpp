#ifndef __LINSUMLAG_HPP__
#define __LINSUMLAG_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class LinsumLag : public LipzObjective<double> {
  public:
    
    LinsumLag(int n)
    {
      setDim(n);
      mRadius = 1.;
      mNLagrangeMult = 1;
    }
    
    double func(double* x)
    {
      return 0.;
    }
        
    void grad(double* x, double* g)
    {
      int n = getDim() - mNLagrangeMult;
      double lam = x[n];
      double v = 0.;
      for(int i = 0; i < n; i ++) {
        g[i] = 1. - 2. * lam * x[i];
        v += x[i] * x[i];
      }
      g[n] = -(v - 1.);
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      return 0.;
    }
    
    double getGradLipzConstant(double* a, double* b)
    {
      int n = getDim() - mNLagrangeMult;
      double maxlam = BNBMAX(BNBABS(a[n]), BNBABS(b[n]));
      double w = 0., v = 0.;
      for(int i = 0; i < n; i ++) {
        double u = maxlam + BNBMAX(BNBABS(a[i]), BNBABS(b[i]));        
        v = BNBMAX(v, u);
        w += u;
      }
      v = 2. * BNBMAX(v, w);
      return v;
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      double v;
      int n = getDim() - mNLagrangeMult;
      if(i < n) {
        double maxlam = BNBMAX(BNBSQR(a[n]), BNBSQR(b[n]));
        double u = BNBMAX(BNBSQR(a[i]), BNBSQR(b[i]));        
        v = 2. * sqrt(maxlam + u);
      } else {
        double u = 0.;
        for(int j = 0; j < n; j ++) {
          u += BNBMAX(BNBSQR(a[j]), BNBSQR(b[j]));
        }
        v = 2. * sqrt(u);
      }
      return v;
    }
    
  private:
    
    double mRadius;
    int mNLagrangeMult;
};
#endif
