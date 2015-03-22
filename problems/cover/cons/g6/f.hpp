#ifndef __F_HPP__
#define __F_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class F :  public LipzObjective<double> {
  
  public:
    
  F()
  {
    Objective<double>::setDim(2);
  }
    
  double func(const double* x)
  {
    double v;
    double u;
    u = x[0] - 10.;
    v = u*u*u;
    u = x[1] - 20.;
    v += u*u*u;
    return v;
  }
        
  void grad(const double* x, double* g)
  {
    g[0] = 3. * (x[0] - 10.) * (x[0] - 10.);
    g[1] = 3. * (x[1] - 20.) * (x[1] - 20.);
  }
    
  double getFuncLipzConstant(double* a, double* b)
  {
    double v = 0.;
    double u;
    u = 3. * BNBMAX((a[0] - 10.) * (a[0] - 10.), (b[0] - 10.) * (b[0] - 10.));
    v += u * u;
    u = 3. * BNBMAX((a[1] - 20.) * (a[1] - 20.), (b[1] - 20.) * (b[1] - 20.));
    v += u * u;    
    return sqrt(v);
  }
    
  double getGradLipzConstant(double* a, double* b)
  {
    double u, v, w;
    u = BNBMAX(BNBABS(a[0] - 10.), BNBABS(b[0] - 10.));    
    w = BNBMAX(BNBABS(a[1] - 20.), BNBABS(b[1] - 20.));
    v = BNBMAX(u, w);
    return  6. * v;
  }
    /*
  double getGradCompLipzConstant(double* a, double* b, int i)
  {
    return 0.;
  }
*/
};

#endif

