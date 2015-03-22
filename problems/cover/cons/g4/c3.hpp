#ifndef __C3_HPP__
#define __C3_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class C3 :  public LipzObjective<double> {
  
  public:
    
    C3()
    {
      Objective<double>::setDim(5);
    }
    
    double func(const double* x)
    {
      double v;
      v = 80.51249 + 0.0071317 * x[1] * x[4] + 0.0029955 * x[0] * x[1] + 0.0021813 * x[2] * x[2];
      Objective<double>::setKind(Objective<double>::INEQCONSTRAINT);
      return v - 110.;
    }
        
    void grad(const double* x, double* g)
    {
      g[0] = 0.0029955 * x[1];
      g[1] = 0.0071317  * x[4] + 0.0029955 * x[0];
      g[2] = 2 * 0.0021813 * x[2];
      g[3] = 0.;
      g[4] = 0.0071317 * x[1];
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double v = 0.;
    
      v += BNBMAX(BNBSQR(0.0029955 * a[1]), BNBSQR(0.0029955 * b[1]));
      v += BNBMAX(BNBSQR(0.0071317  * a[4] + 0.0029955 * a[0]), BNBSQR(0.0071317  * b[4] + 0.0029955 * b[0]));
      v += BNBMAX(BNBSQR(2 * 0.0021813 * a[2]), BNBSQR(2 * 0.0021813 * b[2]));
      v += BNBMAX(BNBSQR(0.0071317 * a[1]), BNBSQR(0.0071317 * b[1]));
    
      return sqrt(v);
    }
    
    double getGradLipzConstant(double* a, double* b)
    {
      return 0.0071317 + 0.0029955 ;
    }
    /*
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
    return 0.;
}
    */
};


#endif

