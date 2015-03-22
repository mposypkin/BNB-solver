#ifndef __C_HPP__
#define __C_HPP__

#include <problems/cover/common/lipzobjective.hpp>
#include "defs.hpp"

class C :  public LipzObjective<FT> {
  
  public:
    
  C(int n, FT* z)
  {
    Objective<FT>::setDim(n);
    mZ = z;
    Objective<FT>::setKind(Objective<FT>::INEQCONSTRAINT);
  }
    
  FT func(const FT* x)
  {
    FT v;
    int n = getDim();
    v = 0.;
    for(int i = 0; i < n; i ++) {
      v += BNBSQR(x[i] - mZ[i]);
    }
    return 1. - v;
  }
        
  void grad(const FT* x, FT* g)
  {
    int n = getDim();
    for(int i = 0; i < n; i ++) {
      g[i] = 2 * (mZ[i] - x[i]);
    }
  }
    
  /*
  FT getFuncLipzConstant(FT* a, FT* b)
  {
    FT v = 0.;
    return sqrt(v);
  }
  */
    
  FT getGradLipzConstant(FT* a, FT* b)
  {
    return 2. ;
  }

  FT getGradCompLipzConstant(FT* a, FT* b, int i)
  {
    return 2.;
  }

  void getEigenBounds(const FT* a, const FT* b, FT& emin, FT& emax)
  {
    emax = -2;
    emin = -2;
  }

  private:

  FT *mZ;
};
#endif

