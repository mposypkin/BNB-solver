#ifndef __F_HPP__
#define __F_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class F :  public LipzObjective<double> {
  
  public:
    
  F()
  {
    Objective<double>::setDim(4);
    mA1 = 0.6224;
    mA2 = 1.7781;
    mA3 = 3.1661;
    mA4 = 19.84;
  }
    
  double func(const double* x)
  {
    double v;
    v = mA1 * x[0] * x[2] * x[3];
    v += mA2 * x[1] * x[2] * x[2];
    v += mA3 * x[0] * x[0] * x[3];
    v += mA4 * x[0] * x[0] * x[2];
    return v;
  }
        
  void grad(const double* x, double* g)
  {
    g[0] = mA1 * x[2] * x[3] + 2. * mA3 * x[0] * x[3] + 2. * mA4 * x[0] * x[2] ;
    g[1] = mA2 * x[2] * x[2] ;
    g[2] = mA1 * x[0] * x[3] + 2. * mA2 * x[1] * x[2] + mA4 * x[0] * x[0];
    g[3] = mA1 * x[0] * x[2] + mA3 * x[0] * x[0];
  }
    
  double getFuncLipzConstant(double* a, double* b)
  {
    BNB_ERROR_REPORT("NOT IMPLEMENTED");
  }
    
  void getEigenBounds(const double* a, const double* b, double& emin, double& emax)
  {
    double l;
    double u;
    double t;
    emin = emax = 0;
    l = 2 * mA3 * a[3] + 2 * mA4 * a[2];
    u = 2 * mA3 * b[3] + 2 * mA4 * b[2];
    t = BNBMAX(BNBABS(mA1 * a[3] + 2. * mA4 * a[0]),BNBABS(mA1 * b[3] + 2. * mA4 * b[0]));    
    l -= t;
    u += t;    
    t =  BNBMAX(BNBABS(mA1 * a[2] + 2. * mA3 * a[0]),BNBABS(mA1 * b[2] + 2. * mA3* b[0]));
    l -= t;
    u += t;
    emin = BNBMIN(l, emin);
    emax = BNBMAX(u, emax);
    
    l = 2 * mA2 * a[2];
    u = 2 * mA2 * b[2];
    emin = BNBMIN(l, emin);
    emax = BNBMAX(u, emax);

    l = 2. * mA2 * a[1];
    u = 2. * mA2 * b[1];
    t = BNBMAX(BNBABS(mA1 * a[3] + 2. * mA4 * a[0]),BNBABS(mA1 * b[3] + 2. * mA4 * b[0]));    
    l -= t;
    u += t;    
    t = BNBMAX(BNBABS(2. * mA2 * a[2]), BNBABS(2. * mA2 * b[2]));
    l -= t;
    u += t;    
    t = BNBMAX(BNBABS(mA1 * a[0]), BNBABS(mA1 * b[0]));
    l -= t;
    u += t;    
    emin = BNBMIN(l, emin);
    emax = BNBMAX(u, emax);
    
    l = u = 0;
    t =  BNBMAX(BNBABS(mA1 * a[2] + 2. * mA3 * a[0]),BNBABS(mA1 * b[2] + 2. * mA3 * b[0]));
    l -= t;
    u += t;    
    t = BNBMAX(BNBABS(mA1 * a[0]), BNBABS(mA1 * b[0]));
    l -= t;
    u += t;    
    emin = BNBMIN(l, emin);
    emax = BNBMAX(u, emax);
  }
   
  private:
    
    double mA1, mA2, mA3, mA4;
};

#endif
