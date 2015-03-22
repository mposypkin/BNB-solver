#ifndef __F_HPP__
#define __F_HPP__

#include <common/lipzobjective.hpp>

class F :  public LipzObjective<double> {
  
  public:
    
  F()
  {
    Objective<double>::setDim(3);
  }
    
  double func(const double* x)
  {
//    printf("Calculating f %f\n", x[0]);
    return x[0];
  }
        
  void grad(const double* x, double* g)
  {
    g[0] = 1;
    g[1] = 0;
    g[2] = 0;
  }
    
  double getFuncLipzConstant(const double* a, const double* b)
  {
    return 1;
  }
    
  /*
  double getGradLipzConstant(double* a, double* b)
  {
    return  0;
  }
  */

  void getEigenBounds(const double* a, const double* b, double& emin, double& emax)
  {
    emax = emin = 0;
  }
    /*
  double getGradCompLipzConstant(double* a, double* b, int i)
  {
    return 0.;
  }
*/
};

#endif

