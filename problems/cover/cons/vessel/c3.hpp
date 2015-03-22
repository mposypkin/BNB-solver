#ifndef __C3_HPP__
#define __C3_HPP__

#include <math.h>
#include <problems/cover/common/lipzobjective.hpp>

class C3 :  public LipzObjective<double> {
  
  public:
    
    C3()
    {
      Objective<double>::setDim(4);
      Objective<double>::mKind = Objective<double>::INEQCONSTRAINT;
      mA1 = - M_PI;
      mA2 = - (4. * M_PI) / 3.;
    }
    
    double func(const double* x)
    {
      double v;
      v = mA1 * x[2] * x[2] * x[3] + mA2 * x[2] * x[2] * x[2] + 1296000.;
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      g[0] = 0.;
      g[1] = 0.;
      g[2] = 2. * mA1 * x[2] * x[3] + 3. * mA2 * x[2] * x[2] ;
      g[3] = mA1 * x[2] * x[2];
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      BNB_ERROR_REPORT("NOT IMPLEMENTED");    
    }
    
    void getEigenBounds(const double* a, const double* b, double& emin, double& emax)
    {
      double l, u, t;
      emin = emax = 0;
      l = 2 * mA1 * a[3] + 6 * mA2 * a[2];
      u = 2. * mA1 * b[3] + 6 * mA2 * b[2];
      t = BNBMAX(BNBABS(2. * mA1 * a[2]), BNBABS(2. * mA1 * b[2]));
      l -= t;
      u += t;
      emin = BNBMIN(l, emin);
      emax = BNBMAX(u, emax);

      l = 2. * mA1 * a[2];
      u = 2. * mA1 * b[2];
      emin = BNBMIN(l, emin);
      emax = BNBMAX(u, emax);
    }

    
  private:
    double mA1, mA2;
    /*
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
    return 0.;
}
    */
};


#endif

