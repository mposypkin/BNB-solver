#ifndef __C5_HPP__
#define __C5_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class C5 :  public LipzObjective<double> {
  
  public:
    
    C5()
    {
      Objective<double>::setDim(5);
      Objective<double>::setKind(Objective<double>::INEQCONSTRAINT);
    }
    
    double func(const double* x)
    {
      double v;
      v = 9.300961 + 0.0047026 * x[2] * x[4] + 0.0012547 * x[0] * x[2] + 0.0019085 * x[2] * x[3];
      return v - 25.;
    }
        
    void grad(const double* x, double* g)
    {
      g[0] = 0.0012547 * x[2];
      g[1] = 0.;
      g[2] = 0.0047026 * x[4] + 0.0012547 * x[0] + 0.0019085 * x[3];
      g[3] = 0.0019085 * x[2];
      g[4] = 0.0047026 * x[2] ;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double v = 0.;
    
      v += BNBMAX(BNBSQR(0.0012547 * a[2]), BNBSQR(0.0012547 * b[2]));
      v += BNBMAX(BNBSQR(0.0047026 * a[4] + 0.0012547 * a[0] + 0.0019085 * a[3]), BNBSQR(0.0047026 * b[4] + 0.0012547 * b[0] + 0.0019085 * b[3]));
      v += BNBMAX(BNBSQR(0.0019085 * a[2]), BNBSQR(0.0019085 * b[2]));
      v += BNBMAX(BNBSQR(0.0047026 * a[2]), BNBSQR(0.0047026 * b[2]));
    
      return sqrt(v);
    }
    
    double getGradLipzConstant(double* a, double* b)
    {
      return 0.0047026  + 0.0012547  + 0.0019085;
    }
    /*
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
    return 0.;
}
    */
};


#endif

