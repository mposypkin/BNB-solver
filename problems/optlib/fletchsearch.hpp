#ifndef __FLETCH_SEARCH_HPP__
#define __FLETCH_SEARCH_HPP__

#include "linesearch.hpp"

template <typename FT> class FletchSearch : public LineSearch <FT>{
  public:
    
    FletchSearch()
    {
      mX = NULL;
      mG = NULL;
      mRightEnd = 10.;
      mTau = 0.1;
      mRho = 0.1;
      mSigma = 0.3;
      mMaxIters = 256;
      mEps = 0.0001;
    }
    
    ~FletchSearch()
    {
      if(mX)
        free(mX);
      if(mG)
        free(mG);
    }
    
    void setTau(FT tau)
    {
      mTau = tau;
    }
    
    void setRho(FT rho)
    {
      mRho = rho;
    }
    
    void setSigma(FT sigma)
    {
      mSigma = sigma;
    }
    
    void setRightEnd(FT re)
    {
      mRightEnd = re;
    }
    
    void setObjective(Objective<FT>* obj)
    {
      mObj = obj;
      mX = (FT*)realloc(mX, sizeof(FT) * mObj->getDim());
      mG = (FT*)realloc(mG, sizeof(FT) * mObj->getDim());
    }

     
    FT search(FT* x, FT* dir)
    {
     FT a, b, c, FA, fa, fb, fc, da, dc, db, df, delt, dnorm;
     int i;
     const int maxEnlarge = 1000;
     const double reduce = 0.0001;
     
     dnorm = VecUtils::vecNormTwo(mObj->getDim(), dir);
     for(int i = 0; i < mObj->getDim(); i ++)
       dir[i] /= dnorm;
     FA = fa = mObj->func(x);
     mObj->grad(x, mG);
     a = 0;
     b = mRightEnd;
     df = da = VecUtils::vecScalarMult(mObj->getDim(), mG, dir);
      i = 0;
     if(da > 0) {
       VecUtils::revert(mObj->getDim(), dir);
       da *= -1;
       df = da;       
     } 
     if(BNBABS(da) <= mEps) {
       return a;
     }     
     for(;;) {
       FT f;
       i ++;
       VecUtils::vecSaxpy(mObj->getDim(), x, dir, b, mX);
       fb = mObj->func(mX);
       if(fb > fa + mRho * da * (b - a))
         break;
       BNB_ASSERT(i < maxEnlarge);
       b = a + 2 * (b - a);
     }
     i = 0;
     for(;;) {
       i ++;
       c = a - 0.5 * (b - a) / ((fb - fa) / ((b - a) * df) - 1.);
       delt = reduce * (b - a);
       if((c < a + delt ) || (c > b - delt))
         c = a + .5 * (b - a);         
       VecUtils::vecSaxpy(mObj->getDim(), x, dir, c, mX);
       fc = mObj->func(mX);
       if(i > mMaxIters) 
         break;
       if(fc > FA + mRho * da * c) {
         // TMP!!!
         mObj->grad(mX, mG);
         double tdc = VecUtils::vecScalarMult(mObj->getDim(), mG, dir);
         b = c;
         fb = fc;
       } else {
         mObj->grad(mX, mG);
         dc = VecUtils::vecScalarMult(mObj->getDim(), mG, dir);
         if(dc > 0) {
           b = c;
           fb = fc;
         } else if(dc > mSigma * da)
           break;
         else {
           a = c;
           fa = fc;
           df = dc;
         }
       }
     }
     return c;
    }
    
    void setEps(FT eps)
    {
      mEps = eps;
    }
  private:
    FT* mG;
    FT* mX;
    FT mRightEnd;
    FT mRho;
    FT mSigma;
    FT mTau;
    FT mEps;
    Objective<FT> *mObj;
    int mMaxIters;
};
#endif
