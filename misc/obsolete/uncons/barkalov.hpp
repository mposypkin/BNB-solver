#ifndef __BARKALOV_HPP__
#define __BARKALOV_HPP__

#include <math.h>
#include <util/common/interval.hpp>

class Barkalov : public LipzObjective<double> {
  public:
    
    Barkalov(int n, double a)
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
        v += x[i] * x[i] - cos(mA * x[i] * x[i]);
      }
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      int n;
      n = getDim();
      for(int i = 0; i < n; i ++) {
        g[i] = 2 * x[i] + 2 * mA * x[i] * sin(mA * x[i] * x[i]);
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
      double vmin, vmax, v, u;
      double maxax, minax;
      double maxsin, minsin;
      double maxcos, mincos;
      BNBInterval<double>::sqr(a[i], b[i], &minax, &maxax);
      maxax *= mA;
      minax *= mA;
      vmin = vmax = 2;
      BNBInterval<double>::sin(minax, maxax, &minsin, &maxsin);
      vmin += 2 * mA * minsin;
      vmax += 2 * mA * maxsin;
      BNBInterval<double>::cos(minax, maxax, &mincos, &maxcos);
      BNBInterval<double>::mult(minax, maxax, mincos, maxcos, &u, &v);
      vmin += 4 * mA * u;
      vmax += 4 * mA * v;
      v = BNBMAX(BNBABS(vmin), BNBABS(vmax));
      return v;
    }

  private:

    double mA;
};
#endif
