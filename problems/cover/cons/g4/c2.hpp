#ifndef __C2_HPP__
#define __C2_HPP__

class C2 :  public LipzObjective<double> {
  
  public:
    
    C2()
    {
      Objective<double>::setDim(5);
      Objective<double>::setKind(Objective<double>::INEQCONSTRAINT);
    }
    
    double func(const double* x)
    {
      double v;
      v = - 85.334407 - 0.0056858 * x[1] * x[4] - 0.0006262 * x[0] * x[3] + 0.0022053 * x[2] * x[4];
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      g[0] = -0.0006262 * x[3];
      g[1] = -0.0056858 * x[4];
      g[2] = 0.0022053 * x[4];
      g[3] = -0.0006262 * x[0];
      g[4] = -0.0056858 * x[1] + 0.0022053 * x[2];
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double v = 0.;
    
      v += BNBMAX(BNBSQR(0.0006262 * a[3]), BNBSQR(0.0006262 * b[3]));
      v += BNBMAX(BNBSQR(0.0056858 * a[4]), BNBSQR(0.0056858 * b[4]));
      v += BNBMAX(BNBSQR(0.0022053 * a[4]), BNBSQR(0.0022053 * b[4]));
      v += BNBMAX(BNBSQR(0.0006262 * a[0]), BNBSQR(0.0006262 * b[0]));
      v += BNBMAX(BNBSQR(0.0056858 * a[1] - 0.0022053 * b[2]), BNBSQR(0.0056858 * b[1] - 0.0022053 * a[2]));
   
      return sqrt(v);
    }
    
    double getGradLipzConstant(double* a, double* b)
    {
      return 0.0056858 + 0.0022053 ;
    }
    /*
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
    return 0.;
}
    */
};


#endif

