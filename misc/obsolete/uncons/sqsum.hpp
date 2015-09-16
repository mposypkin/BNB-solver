#ifndef __SQSUM_HPP__
#define __SQSUM_HPP__

class Sqsum : public LipzObjective<double> {
  public:
    
    Sqsum(int n)
    {
      mDim = n;
    }

    double func(const double* x)
    {
      double v;
      v = 0.;
      for(int i = 0; i < getDim(); i ++)
        v += x[i] * x[i];
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      for(int i = 0; i < getDim(); i ++)
        g[i] = 2 * x[i];
    }
    
    double getFuncLipzConstant(const double* a, const double* b)
    {
      double norm, l, u;
      norm = 0.;
      for(int i = 0; i < getDim(); i ++) {
        l = a[i];
        u = b[i];
        norm += 4 * BNBMAX(l * l, u * u);
      }      
      return sqrt(norm);
      //return 2;
    }
    
    void getEigenBounds(const double* a, const double* b, double& emin, double& emax)
    {
      emax = 2;
      emin = 2;
    }
    
    double getGradCompLipzConstant(const double* a, const double* b, int i)
    {
      return 2.;
    }

};
#endif
