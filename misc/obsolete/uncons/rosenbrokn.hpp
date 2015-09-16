#ifndef __ROSENBROKN_HPP__
#define __ROSENBROKN_HPP__

#include <util/common/vec.hpp>
#include <util/common/interval.hpp>

class Rosenbrokn : public LipzObjective<double> {
  public:
    
    Rosenbrokn(int n)
    {
      mDim = n;
    }

    double func(const double* x)
    {
      double v;
      int n;
      v = 0.;
      n = getDim();
      for(int i = 0; i < (n - 1); i ++) {
        v += 100. * BNBSQR(x[i] * x[i] - x[i + 1]) + BNBSQR(x[i] - 1);
      }
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      int n;
      n = getDim();
      for(int i = 0; i < n; i ++) {
        double v;
        v = 0.;
        if(i != 0)
          v += 200. * (x[i] - x[i - 1] * x[i - 1]);
        if(i != (n - 1))
          v += 400. * x[i] * x[i] * x[i] - 400. * x[i] * x[i + 1] + 2. * x[i] - 2.;
        g[i] = v;
      }
    }
    

    int hessn(const double* x, double* H)
    {
      int n;
      n = getDim();
      VecUtils::vecSet(n * n, 0., H);
      for(int i = 0; i < n; i ++) {
        int m = i * n + i;
        if(i != (n -1)) {
          H[m] += 1200. * x[i] * x[i] - 400. * x[i + 1] + 2.;          
          H[m + 1] = - 400. * x[i];
        }
        if(i != 0) {
          H[m] += 200.;
          H[m - 1] = - 400. * x[i - 1];
        }
      }
      return n;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      int n;
      double norm;
      norm = 0.;
      n = getDim();
      for(int i = 0; i < n; i ++) {
        double v;
        double xm;
        
        v = 0.;
        xm = BNBMAX(BNBABS(a[i]), BNBABS(b[i]));
        if(i != 0) {
          double xmp;
          xmp = BNBMAX(BNBABS(a[i - 1]), BNBABS(b[i - 1]));
          v += 200. * (xm + xmp * xmp);
        }
        if(i != (n - 1)) {
          double xmn;
          xmn = BNBMAX(BNBABS(a[i + 1]), BNBABS(b[i + 1]));
          v += 400. * xm * xm * xm + 400. * xm * xmn + 2. * xm + 2.;
        }
        norm += BNBSQR(v);
      }
      return sqrt(norm);
    }
    
    void getEigenBounds(const double* a, const double* b, double& emin, double& emax)
    {
      int n;
      n = getDim();
      emax = getMinValue<double>();
      emin = getMaxValue<double>();
      for(int i = 0; i < n; i ++) {
	double lemax;
	double lemin;
	double dlo = 0;
	double dup = 0;
        if(i != (n - 1)) {
	  BNBInterval<double>::sqr(a[i], b[i], &dlo, &dup);
	  BNBInterval<double>::diff(1200*dlo, 1200*dup, 400*a[i+1], 400*a[i+1], &dlo, &dup);
	  dlo += 2;
	  dup += 2;
        }
        if(i != 0) {
	  dlo += 200;
	  dup += 200;
        }
	lemin = dlo;
	lemax = dup;
	if(i != (n - 1)) {
	  double xabs = BNBMAX(BNBABS(a[i]), BNBABS(b[i]));
	  lemin -= 400 * xabs;
	  lemax += 400 * xabs;
	}
	if(i != 0) {
	  double xabs = BNBMAX(BNBABS(a[i - 1]), BNBABS(b[i - 1]));
	  lemax += 400 * xabs;
	  lemin -= 400 * xabs;
	}
	emin = BNBMIN(emin, lemin);
	emax = BNBMAX(emax, lemax);
      }
    }
    
    double getGradCompLipzConstant(const double* a, const double* b, int i)
    {
      int n;
      double lc, xm, xn, xp, t;
      
      lc = 0.;
      n = getDim();
      xm = BNBMAX(BNBABS(a[i]), BNBABS(b[i]));
      xn = (i == (n - 1)) ? 0. : (BNBMAX(BNBABS(a[i + 1]), BNBABS(b[i + 1])));
      xp = (i == 0) ? 0. : (BNBMAX(BNBABS(a[i - 1]), BNBABS(b[i - 1])));
      t = (i == (n - 1)) ? 0. : (1200. * xm * xm + 400. * xn + 2);
      t += (i == 0) ? 0. : 200.;
      lc += t * t;
      lc += (i == (n - 1))  ? 0. : BNBSQR(400. * xm);
      lc += (i == 0) ? 0. : BNBSQR(400. * xp);
      lc = sqrt(lc);
      return lc;
    }

    double getHessLipzConstant(double* a, double* b)
    {
      double v = 0.;
      int n = getDim();
      
      for(int i = 0; i < (n - 1); i ++) {
        double t = BNBMAX(BNBABS(a[i]), BNBABS(b[i]));
        v = BNBMAX(v, 400. * (sqrt(36. * t + 1.) + 2.));
      }
      return v;
    }

};
#endif
