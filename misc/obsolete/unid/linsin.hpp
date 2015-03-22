#ifndef __LINSIN_HPP__
#define __LINSIN_HPP__

class Linsin : public LipzObjective<double> {
  public:
    
    Linsin ()
    {
      setDim(1);
//       mA = 1.;
//       mB = 5.;
      mA = 1.;
      mB = 5.;
    }
    
    double func(double* x)
    {
      double y = x[0];
      double v = mA * y + sin(mB * y);
      return v;
    }
        
    void grad(double* x, double* g)
    {
      double y = x[0];
      double v = mA + mB * cos(mB * y);
      g[0] = v;
    }
    
    int hessn(double* x, double* H)
    {
      double y = x[0];
      H[0] = - mB * mB * sin(mB * y);
      return 1;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double l, u;
      BNBInterval<double>::cos(mB * a[0], mB * b[0], &l, &u);
      BNBInterval<double>::sum(mA, mA, mB * l, mB * u, &l, &u);
      return BNBMAX(BNBABS(l), BNBABS(u));      
    }
   
    double getGradLipzConstant(double* a, double* b)
    {
      double l, u, q = mB * mB;
      BNBInterval<double>::sin(mB * a[0], mB * b[0], &l, &u);
      return q * BNBMAX(BNBABS(l), BNBABS(u));      
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      return getGradLipzConstant(a, b);
    }
    
    double getHessLipzConstant(double* a, double* b)
    {
      double l, u, p = mB * mB * mB;
      BNBInterval<double>::cos(mB * a[0], mB * b[0], &l, &u);
      return p * BNBMAX(BNBABS(l), BNBABS(u));      
    }
    
  private:
    
    double mA, mB;

};
#endif
