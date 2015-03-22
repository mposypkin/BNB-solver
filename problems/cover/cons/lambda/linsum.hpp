#ifndef __LINSUM_HPP__
#define __LINSUM_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class Linsum : public LipzObjective<double> {
  public:
    
    Linsum(int n, int lagmult = 1)
    {
      Objective<double>::setDim(n);
      mRadius = 1.;
      mNLagrangeMult = lagmult;
    }
    
    double func(double* x)
    {
      double v;
      int n = getDim() - mNLagrangeMult;
      v = 0.;
      for(int i = 0; i < n; i ++)
        v += x[i];
      return v;
    }
        
    void grad(double* x, double* g)
    {
      int n = getDim() - mNLagrangeMult;
      for(int i = 0; i < n; i ++) {
        g[i] = 1.;
      }
      if(mNLagrangeMult == 1)
        g[n] = 0.;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      return sqrt(getDim());
    }
    
    double getGradLipzConstant(double* a, double* b)
    {
      return 0.;
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      return 0.;
    }
    
  private:
    

    double mRadius;
    int mNLagrangeMult;
};
#endif
