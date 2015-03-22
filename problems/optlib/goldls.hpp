#ifndef __GOLDEN_LINE_SEARCH__
#define __GOLDEN_LINE_SEARCH__

#include <util/common/vec.hpp>
#include "linesearch.hpp"


template <typename FT> class GoldenLineSearch : public LineSearch <FT> {
  
  public:
    
    GoldenLineSearch()
    {
      mX = NULL;
    }
    
    ~GoldenLineSearch()
    {
      if(mX)
        free(mX);
    }
    
    void setObjective(Objective<FT>* obj)
    {
      mObj = obj;
      mX = (FT*)realloc(mX, sizeof(FT) * mObj->getDim());
    }
    
    FT search(FT* x, FT* dir)
    {
      FT vb, vc, vd, vx;
      FT a, b, c, d;
      const FT tau = 1.618;
      
      a = 0.;
      d = 1.;
      b = d /(1. + tau);
      vx = mObj->func(x);
      VecUtils::vecSaxpy(mObj->getDim(), x, dir, -d, mX);
      vd = mObj->func(mX);
      do {
        if(vd > vx) {
          b = d /(1. + tau);
          VecUtils::vecSaxpy(mObj->getDim(), x, dir, -b, mX);          
          vb = mObj->func(mX);
          if(vb < vx)
            break;
          else {
            d = b;
            vd = vb;
          }
        } else if(vd < vx) {
          b = d;
          vb = vd;
          d *= (1. + tau);
          VecUtils::vecSaxpy(mObj->getDim(), x, dir, -d, mX);
          vd = mObj->func(mX);
          if(vd >= vb) 
            break;
          else {
            if(d > mMaxAlpha)
              return d;
          }
        } else
          break;
      } while(1);
      //printf("GLS: %lf (%lf), %lf(%lf), %lf(%lf) \n", a, vx, b, vb, d, vd);
      do {
        b = a + (d - a) / (1. + tau);
        c = b + (d - b) / (1. + tau);
        //printf("1: %lf, %lf, %lf, %lf\n", a, b, c, d);
        VecUtils::vecSaxpy(mObj->getDim(), x, dir, -b, mX);
        vb = mObj->func(mX);
        VecUtils::vecSaxpy(mObj->getDim(), x, dir, -c, mX);
        vc = mObj->func(mX);
        if(vb <= vc) 
          d = c;
        else if(vb > vc) {
          a = b;
          b = c;
        }
        //printf("%lf, %lf, %lf, %lf\n", a, b, c, d);
      } while(d - a > mDelta);
      return 0.5 * (a + d);
    }

    void setMaxAlpha(FT a)
    {
      mMaxAlpha = a;
    }
    
    void setDelta(FT d)
    {
      mDelta = d;
    }
    
  private:
    Objective<FT> *mObj;
    FT* mX;
    FT mMaxAlpha;
    FT mDelta;
};

#endif
