#ifndef __F_HPP__
#define __F_HPP__

#include <problems/cover/common/lipzobjective.hpp>
#include "defs.hpp"

class F :  public LipzObjective<FT> {
  
  public:
    
  F(int n)
  {
    Objective<FT>::setDim(n);
  }
    
  FT func(const FT* x)
  {
    int n = getDim();
    FT v;
    v = 0.;
    for(int i = 0; i < n; i ++) 
      v += x[i] * x[i];
    return -v;
  }
        
  void grad(const FT* x, FT* g)
  {
    int n = getDim();
    for(int i = 0; i < n; i ++) {
      g[i] = -2 * x[i];
    }
  }
    
  /*
  FT getFuncLipzConstant(FT* a, FT* b)
  {
    FT v = 0.;
    FT u;
    u =  BNBMAX(BNBABS(0.8356891 * a[4] + 37.293239), BNBABS(0.8356891 * b[4] + 37.293239));
    v += u * u;
    u =  BNBMAX(BNBABS(2. * 5.3578547 * a[2]), BNBABS(2. * 5.3578547 * b[2]));
    v += u * u;    
    u =  BNBMAX(BNBABS(0.8356891 * a[0]), BNBABS(0.8356891 * b[0]));
    v += u * u;    
    return sqrt(v);
  }
  */
    
  FT getGradLipzConstant(FT* a, FT* b)
  {
    return  2.;
  }
    
  FT getGradCompLipzConstant(FT* a, FT* b, int i)
  {
    return 2.;
  }

  void getEigenBounds(const FT* a, const FT* b, FT& emin, FT& emax)
  {
    emin = -2;
    emax = -2;
  }
};

#endif

