#ifndef __C1_HPP__
#define __C1_HPP__

#include <common/lipzobjective.hpp>

class C1 :  public LipzObjective<double> {
  
  public:
    
  C1()
  {
    Objective<double>::setDim(3);
    Objective<double>::setKind(Objective<double>::INEQCONSTRAINT);
    mDelt = 0.0;
  }
    
  double func(const double* x)
  {
    double v;
    v = BNBSQR(x[0] - 5.) + 2*BNBSQR(x[1] - 5.) + BNBSQR(x[2] - 5) - 18 + mDelt;
    return v;
  }
        
  void grad(const double* x, double* g)
  {
    g[0] = 2. * (x[0] - 5.);
    g[1] = 4. * (x[1] - 5.);
    g[2] = 2. * (x[2] - 5.);
  }

  void setDelt(double delt)
  {
    mDelt = delt;
  }
    
  double getFuncLipzConstant(const double* a, const double* b)
  {
    double l, u, U;
    l = 2*(a[0] - 5), u = 2*(b[0] - 5);
    BNBInterval<double>::sqr(l, u, &l, &u);
    U = u;
    l = 4*(a[1] - 5), u = 4*(b[1] - 5);
    BNBInterval<double>::sqr(l, u, &l, &u);
    U += u;
    l = 2*(a[2] - 5), u = 2*(b[2] - 5);
    BNBInterval<double>::sqr(l, u, &l, &u);
    U += u;
    return sqrt(U);
  }
    
  /*
  double getGradLipzConstant(double* a, double* b)
  {
    return 4.;
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
  emax = 4;
  emin = 2;
}


double mDelt;

};


#endif

