#ifndef __RASTRLIP_HPP__
#define __RASTRLIP_HPP__

#include <math.h>
#include <util/common/interval.hpp>

/**
  * Rastringin function
  */
class RastrLip: public LipzObjective<double> {
  public:
    
    RastrLip(int n, double a)
    {
      mDim = n;
      mA = a;
    }

    double func(const double* x)
    {
      double v;
      int n;
      v = 0.;
      n = getDim();
      for(int i = 0; i < n; i ++) {
        v += x[i] * x[i] - mA * cos(2 * M_PI * x[i]);
      }
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      int n;
      n = getDim();
      for(int i = 0; i < n; i ++) {
        g[i] = 2 * x[i] + 2 * M_PI * mA * sin(2 * M_PI * x[i]);
      }
    }

    double getGradLipzConstant(double* a, double* b)
    {
      int n;
      double maxv;
      
      maxv= 0.;
      n = getDim();
      for(int i = 0; i < n; i ++) {
        double v;
        v = getGradCompLipzConstant(a, b, i);
	maxv = BNBMAX(v, maxv);
      }
      return maxv;
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      double v, t;
      double maxsin, minsin;
      BNBInterval<double>::cos(2 * M_PI * a[i], 2 * M_PI * b[i], &minsin, &maxsin);
      t = 4 * M_PI * M_PI * mA;
      minsin *= t;
      maxsin *= t;
      minsin += 2;
      maxsin += 2;
      v = BNBMAX(BNBABS(maxsin), BNBABS(minsin));
      return v;
    }

  private:

    double mA;
};
#endif
