#ifndef __C1_HPP__
#define __C1_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class C1 :  public LipzObjective<double> {
  
  public:
    
  C1()
  {
    Objective<double>::setDim(4);
    Objective<double>::mKind = Objective<double>::INEQCONSTRAINT;
    mTol = 0.;
  }
    
  void setTolerance(double tol)
  {
    mTol = tol;
  }

  double func(const double* x)
  {
    double v;
    v = -x[0] + 0.0193 * x[2];
    return v + mTol;
  }
        
  void grad(const double* x, double* g)
  {
    g[0] = -1.;
    g[1] = 0.;
    g[2] = 0.0193;
    g[3] = 0.;
  }
    
  double getFuncLipzConstant(double* a, double* b)
  {
    BNB_ERROR_REPORT("NOT IMPLEMENTED");
  }
    
  double getGradLipzConstant(double* a, double* b)
  {
    return 0.;
  }

  private:

  double mTol;
 };


#endif

