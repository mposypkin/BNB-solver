#ifndef __C2_HPP__
#define __C2_HPP__

#include <common/lipzobjective.hpp>

class C2 :  public LipzObjective<double> {
  
  public:
    
    C2()
    {
      Objective<double>::setDim(3);
      Objective<double>::setKind(Objective<double>::INEQCONSTRAINT);
      mDelt = 0.0;
    }
    
    double func(const double* x)
    {
      double v;
      v = 100 - BNBSQR(x[0] - 2*x[1] + 7) - 4*BNBSQR(2*x[0] + x[1] - 11) - 5* BNBSQR(x[2] - 5) + mDelt;
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      g[0] = -2*(x[0] - 2*x[1] + 7) - 16*(2*x[0] + x[1] - 11);
      g[1] = 4*(x[0] - 2*x[1] + 7) - 8*(2*x[0] + x[1] - 11);
      g[2] = -10*(x[2] - 5);
    }

    void setDelt(double delt)
    {
      mDelt = delt;
    }

    
    double getFuncLipzConstant(const double* a, const double* b)
    {
      double l, u, U;
      l = -34 * b[0] - 12 * b[1] + 162;
      u = -34 * a[0] - 12 * a[1] + 162;
      BNBInterval<double>::sqr(l, u, &l, &u);
      U = u;

      l = -12 * b[0] - 16 * b[1] + 116;
      u = -12 * a[0] - 16 * a[1] + 116;
      BNBInterval<double>::sqr(l, u, &l, &u);
      U += u;

      l = -10 * b[2] + 50;
      u = -10 * a[2] + 50;
      BNBInterval<double>::sqr(l, u, &l, &u);
      U += u;

      return sqrt(U);
    }
    
    /*
    double getGradLipzConstant(double* a, double* b)
    {
      return 46.;
    }
    */
    /*
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
    return 0.;
}
    */
void getEigenBounds(const double* a, const double* b, double& emin, double& emax)
{
  emax = -4;
  emin = -46;
}

double mDelt;

};


#endif

