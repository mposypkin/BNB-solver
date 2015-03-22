#ifndef __F_HPP__
#define __F_HPP__

#include <problems/cover/common/lipzobjective.hpp>

class F :  public LipzObjective<double> {
  
  public:
    
  F()
  {
    Objective<double>::setDim(5);
  }
    
  double func(const double* x)
  {
    double v;
    v = 5.3578547 * x[2] * x[2];
    v += 0.8356891 * x[0] * x[4];
    v += 37.293239 * x[0];
    v -= 40792.141;
    return v;
  }
        
  void grad(const double* x, double* g)
  {
    g[0] = 0.8356891 * x[4] + 37.293239;
    g[1] = 0. ;
    g[2] = 2. * 5.3578547 * x[2];
    g[3] = 0.;
    g[4] = 0.8356891 * x[0];
  }
    
  double getFuncLipzConstant(double* a, double* b)
  {
    double v = 0.;
    double u;
    u =  BNBMAX(BNBABS(0.8356891 * a[4] + 37.293239), BNBABS(0.8356891 * b[4] + 37.293239));
    v += u * u;
    u =  BNBMAX(BNBABS(2. * 5.3578547 * a[2]), BNBABS(2. * 5.3578547 * b[2]));
    v += u * u;    
    u =  BNBMAX(BNBABS(0.8356891 * a[0]), BNBABS(0.8356891 * b[0]));
    v += u * u;    
    return sqrt(v);
  }
    
  double getGradLipzConstant(double* a, double* b)
  {
    return  2. * 5.3578547;
  }
    /*
  double getGradCompLipzConstant(double* a, double* b, int i)
  {
    return 0.;
  }
*/
};

#endif

