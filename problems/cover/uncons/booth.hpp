#ifndef __BOOTH_HPP__
#define __BOOTH_HPP__

class Booth : public LipzObjective<double> {
  public:
    
    Booth() 
    {
      mDim = 2;
    }

    double func(const double* x)
    {
      double v;
      v = BNBSQR(x[0] + 2. * x[1] - 7) + BNBSQR(2.* x[0] + x[1] - 5.);
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      g[0] = 10. * x[0] + 8. * x[1] - 34.;
      g[1] = 8. * x[0] + 10. * x[1] - 38.;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double norm, l, u;
      norm = 0.;
      l = 10. * a[0] + 8. * a[1] - 34.;
      u = 10. * b[0] + 8. * b[1] - 34.;
      norm += BNBMAX(l * l, u * u);
      
      l = 8. * a[0] + 10. * a[1] - 38.;
      u = 8. * b[0] + 10. * b[1] - 38.;
      norm += BNBMAX(l * l, u * u);
      
      return sqrt(norm);
    }
    
    void getEigenBounds(const double* a, const double* b, double& emin, double& emax)
    {
      emax = 18;
      emin = 18;
    }

    double getGradCompLipzConstant(const double* a, const double* b, int i)
    {
      return 13.;
    }
    
    double getHessLipzConstant(double* a, double* b)
    {
      return 0.00000001;
    }

    int hessn(const double* x, double* H)
    {
      H[0] = 10.;
      H[1] = 8.;
      H[2] = 8.;
      H[3] = 10.;
      return 2;
    }


};
#endif
