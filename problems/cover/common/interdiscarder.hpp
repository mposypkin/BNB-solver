#ifndef __INTERDISCARDER_HPP__
#define __INTERDISCARDER_HPP__

#include "go.hpp"
#include "intobj.hpp"
#include <util/common/gvector.hpp>
#include <util/common/interval.hpp>
/**
 * A discarder based on interval arithmetic
 */
template <typename FT> class IntervalDiscarder : public GOFactory<FT>::Discarder 
{
  public: 
    struct EvalOptions {
      static const unsigned int EVAL_FUNCTION = (1 << 1);
      static const unsigned int EVAL_GRAD_SIMPLE = (1 << 2);
      static const unsigned int EVAL_FORM1 = (1 << 3);
      static const unsigned int EVAL_HANSEN = (1 << 4);
      static const unsigned int EVAL_POSYP = (1 << 5);
    };
    
    struct ReduceOptions {
      static const unsigned int REDUCE_GRAD_ZERO = (1 << 1);
      static const unsigned int REDUCE_HANSEN_N = (1 << 2);
      static const unsigned int REDUCE_HANSEN_ALL = (1 << 3);
      static const unsigned int REDUCE_HANSEN_ALL_ADVANCED = (1 << 4);
      static const unsigned int REDUCE_HANSEN_ALLCORNER = (1 << 5);
      static const unsigned int REDUCE_HANSEN_BESTCORNER = (1 << 6);
    };

    IntervalDiscarder()
    {
      mEvalOptions = 0;
      mReduceOptions = 0;
      mGradL = NULL;
      mGradU = NULL;
      mMidle = NULL;
      mLB = NULL;
      mUB = NULL;
      mPoint = NULL;
      mEps = 0.;
      mMask = NULL;
      mBestMask = NULL;
      mObj = NULL;    
    }
    
    void setObjective(IntervalObjective<FT>* obj) {
      int n;
      mObj = obj;
      n = mObj->getDim();
      mGradU = (FT*)realloc(mGradU, n * sizeof(FT));
      mGradL = (FT*)realloc(mGradL, n * sizeof(FT));
      mMidle = (FT*)realloc(mMidle, n * sizeof(FT));
      mPoint = (FT*)realloc(mPoint, n * sizeof(FT));
      mLB = (FT*)realloc(mLB, n * sizeof(FT));
      mUB = (FT*)realloc(mUB, n * sizeof(FT));
      mMask = (int*) realloc(mMask, n * sizeof(int));
      mBestMask = (int*) realloc(mBestMask, n * sizeof(int));
    }

    ~IntervalDiscarder()
    {
      if(mGradL)
        free(mGradL);
      if(mGradU)
        free(mGradU);
      if(mMidle)
        free(mMidle);
      if(mLB)
        free(mLB);
      if(mUB)
        free(mUB);
      if(mPoint)
        free(mPoint);
      if(mMask)
        free(mMask);
      if(mBestMask)
        free(mBestMask);
    }
 
    void discard(typename GOFactory<FT>::Set& s, GVector< typename GOFactory<FT>::Set >& sv, GVector< typename GOFactory<FT>::Solution >& solv, FT record, BNBBranchInfo* info) 
    {
      bool fl, rv = true;
      typename std::vector< typename GOFactory<FT>::Set >::iterator i;
      fl = reduce(s, record, mEps);
      if(fl) {
        FT estim;
        estim = eval(s);
        if(estim < record - mEps){
          s.mEstim = estim;
          rv = false;
        }
      }
      if(!rv)
        sv.push_back(s);        
      else
        info->mDiscardedByRecord ++;
    }
  
    /**
     * Set tolerance
     * @param eps tolerance value
     */
    void setEps(FT eps) {
      mEps = eps;
    }
    /**
     * Set up evaluation options
     * @param options options to set
     */
    void setEvalOptions(unsigned int options)
    {
      mEvalOptions = options;
    }
    
    /**
     * Set up reduction options
     * @param options options to set
     */
    void setReduceOptions(unsigned int options)
    {
      mReduceOptions = options;
    }

  private:
    bool reduceHansenN(typename GOFactory<FT>::Set& s, FT record, FT eps) 
    {
      FT c, r;
      int n;
      n = mObj->getDim() - 1;
      c = mObj->func(s.mA);
      //record = BNBMIN(c, record);
      for(int i = 0; i <= n; i ++) 
        mLB[i] = mUB[i] = s.mA[i];

      for(int i = 0; i < n; i ++) {
        FT l, a, b;
        
        l = s.mB[i] - s.mA[i];
        mUB[i] = s.mB[i];
        mObj->gradInt(mLB, mUB, mGradL, mGradU);
        BNBInterval<FT>::mult(0, l, mGradL[i], mGradU[i], &a, &b);
        c += a;
      }
      r = record - c - eps;
      if(r >= 0) 
        return true;
      else {
        FT g;
        
        mUB[n] = s.mB[n];
        mObj->gradInt(mLB, mUB, mGradL, mGradU);
        g = mGradL[n];
        if((g > 0.)||(mGradU[n] < 0.)) 
          return false;
        else if(g == 0.)
          return true;
        else {
          FT d;
          d = r / g;
          if(d > (s.mB[n] - s.mA[n])) 
            return false;            
          else {
            s.mA[n] += d;
            return true;
          }          
        }
      }
    }
    
    bool reduceHansenAll(typename GOFactory<FT>::Set& s, FT record, FT eps, bool advanced, int* mask = NULL) 
    {
      FT c, r, fv;
      int n;
      n = mObj->getDim() - 1;
      for(int i = 0;  i <= n; i ++) {
        if(mask && (mask[i])) 
          mPoint[i] = s.mB[i];
        else
          mPoint[i] = s.mA[i];
      }
      fv = mObj->func(mPoint);
      for(int I = 0; I <= n; I ++) {
        c = fv;
        for(int i = 0; i <= n; i ++) 
          if(mask && (mask[i])) {
          mLB[i] = mUB[i] = s.mB[i];
          } else {
            mLB[i] = mUB[i] = s.mA[i];
          }
          
          for(int i = 0; i <= n; i ++) {
            if(i != I) {
              FT l, a, b, le, re;
        
              l = s.mB[i] - s.mA[i];
              mLB[i] = s.mA[i];
              mUB[i] = s.mB[i];
              mObj->gradInt(mLB, mUB, mGradL, mGradU);
              if(mask && (mask[i])) {
                le = -l;
                re = 0;
              } else {
                le = 0;
                re = l;
              }
              BNBInterval<FT>::mult(le, re, mGradL[i], mGradU[i], &a, &b);
              c += a;
            }
          }
          r = record - c - eps;
          FT g;
          mLB[I] = s.mA[I];
          mUB[I] = s.mB[I];
          mObj->gradInt(mLB, mUB, mGradL, mGradU);
          if((mGradL[I] > 0.)|| (mGradU[I] < 0.)) 
            return false;
          if(mask && mask[I])
            g = mGradU[I];
          else
            g = mGradL[I];
        
          if((r < 0) && (g == 0.)) 
            return false;
          if(r < 0) {
            FT d;
            d = r / g;
            if(BNBABS(d) >= (s.mB[I] - s.mA[I])) 
              return false;
            if(advanced) {
              for(int k = 0; k < 10; k ++){
                mObj->gradInt(mLB, mUB, mGradL, mGradU);
                g = mGradL[I];
                if(g >= 0) {
                  d = mUB[I] - s.mA[I];
                  mUB[I] = 0.5 * (mUB[I] + s.mB[I]);
                } else {
                  d = BNBMIN(r/g, s.mB[I] - s.mA[I]);
                  mUB[I] = .5 * (s.mA[I] + mUB[I] + d);
                }
              } 
              d = BNBMIN(mUB[I] - s.mA[I], d);
            }              
            if(mask && (mask[I]))
              s.mB[I] += d;
            else
              s.mA[I] += d;          
          }
      }
      return true;
    }
    
    bool reduceHansenAllCorner(typename GOFactory<FT>::Set& s, FT record, FT eps) 
    {
      int n;
      n = mObj->getDim();
      for(int i = 0; i < n; i ++) {
        mMask[i] = 0;
      }
      for(;;) {
        bool rv, fl;
        
        //VecUtils::vecPrint(n, mMask);
        rv = reduceHansenAll(s, record, eps, false, mMask);
        if(rv == false)
          return false;
        fl = false;
        for(int i = 0; i < n; i ++) {
          if(mMask[i] == 0) {
            mMask[i] = 1;
            if(i != 0)
              mMask[i - 1] = 0;
            fl = true;
            break;
          }
        }
        if(fl == false)
          return true;
      }
    }
    
    bool reduceHansenBestCorner(typename GOFactory<FT>::Set& s, FT record, FT eps) 
    {
      int n;
      FT bestv;
      bool rv;
      int o;
          
      n = mObj->getDim();
      bestv = getMinValue<FT>();
      o = 0;
      for(int i = 0; i < n; i ++) {
        mMask[i] = 0;
      }      
      for(;;) {
        bool fl;
        FT fv;
        
        for(int i = 0; i < n; i ++) {
          mPoint[i] = mMask[i] ? s.mB[i] : s.mA[i];
        }
        fv = mObj->func(mPoint);
        if(fv > bestv) {
          for(int i = 0; i < n; i ++) 
            mBestMask[i] = mMask[i];
          bestv = fv;
        }
        fl = false;
        for(int i = 0; i < n; i ++) {
          if(mMask[i] == 0) {
            mMask[i] = 1;
            if(i != 0)
              mMask[i - 1] = 0;
            fl = true;
            break;
          }
        }
        if(fl == false)
          break;
      }
      rv = reduceHansenAll(s, record, eps, false, mBestMask);
      return rv;
    }
    
    bool reduceHansenOrig(typename GOFactory<FT>::Set& s, FT record, FT eps) 
    {
      FT c, r, g;
      int n;
      
      n = mObj->getDim() - 1;
      for(int I = 0; I <= n; I ++) {
        c = mObj->func(s.mA);
        for(int i = 0; i <= n; i ++) 
          mLB[i] = mUB[i] = s.mA[i];

        for(int i = 0; i <= n; i ++) {
          mUB[i] = s.mB[i];
          mObj->gradInt(mLB, mUB, mGradL, mGradU);
          if(i != I) {
            FT l, a, b;
        
            l = s.mB[i] - s.mA[i];
            BNBInterval<FT>::mult(0, l, mGradL[i], mGradU[i], &a, &b);
            c += a;
          } else {
            g = mGradL[i];
          }
        }
        
        r = record - c - eps;
        if((r < 0.) && (g < 0.)) {
          FT d;
          d = r / g;
          if(d > (s.mB[I] - s.mA[I])) 
            return false;
          else 
            s.mA[I] += d;
        }
      }
      return true;
    }
    
    bool reduceGradZero(typename GOFactory<FT>::Set& s, FT record, FT eps) 
    {
      mObj->gradInt(s.mA, s.mB, mGradL, mGradU);
      for(int i = 0; i < mObj->getDim(); i ++) {
        if((mGradL[i] > 0.) || (mGradU[i] < .0)) 
          return false;
      }
      return true;
    }
    
    bool reduce(typename GOFactory<FT>::Set& s, FT record, FT eps)
    {
      bool rv = true;
      
      if(mReduceOptions & ReduceOptions::REDUCE_GRAD_ZERO) {
        rv = reduceGradZero(s, record, eps);
      } 
      if(rv && (mReduceOptions & ReduceOptions::REDUCE_HANSEN_N)) {
        //rv = reduceHansenN(s, record, eps);
        rv = reduceHansenOrig(s, record, eps);
      }  
      if(rv && (mReduceOptions & ReduceOptions::REDUCE_HANSEN_ALL)) {
        rv = reduceHansenAll(s, record, eps, false);
      }  
      if(rv && (mReduceOptions & ReduceOptions::REDUCE_HANSEN_ALL_ADVANCED)) {
        rv = reduceHansenAll(s, record, eps, true);
      }
      if(rv && (mReduceOptions & ReduceOptions::REDUCE_HANSEN_ALLCORNER)) {  
        rv =  reduceHansenAllCorner(s, record, eps);
      }
      if(rv && (mReduceOptions & ReduceOptions::REDUCE_HANSEN_BESTCORNER)) {  
        rv =  reduceHansenBestCorner(s, record, eps);
      }
      return rv;
    }

    
    /**
     * Evaluation based on the function interval bound
     * 
     * @param s set to evaluate
     * @param record incumbent
     * @param eps tolerance
     * @return the lower bound
     */
    FT evalFunc(typename GOFactory<FT>::Set &s)
    {
      FT c;
      mObj->funcMin(s.mA, s.mB, &c);
      return c;
    }
    
    FT evalGradSimple(typename GOFactory<FT>::Set &s)
    {
      FT c;
      mObj->gradInt(s.mA, s.mB, mGradL, mGradU);
      for(int i = 0; i < mObj->getDim(); i ++) 
        mMidle[i] = 0.5 * (s.mA[i] + s.mB[i]);
      c = mObj->func(mMidle);
      for(int i = 0; i < mObj->getDim(); i ++) {
        FT l, a, b;
        
        l = 0.5 *(s.mB[i] - s.mA[i]);
        BNBInterval<FT>::mult(-l, l, mGradL[i], mGradU[i], &a, &b);
        c += a;
      }
      return c;
    }
    
    FT evalForm1(typename GOFactory<FT>::Set &s)
    {
      FT c, e;
      for(int i = 0; i < mObj->getDim(); i ++) 
        mMidle[i] = 0.5 * (s.mA[i] + s.mB[i]);
      c = mObj->func(mMidle);
      mObj->form1(s.mA, s.mB, &e);
      return c + e;
    }
    
    FT evalHansen(typename GOFactory<FT>::Set &s) 
    {
      FT c;
      for(int i = 0; i < mObj->getDim(); i ++) {
        mMidle[i] = 0.5 * (s.mA[i] + s.mB[i]);
        mLB[i] = mUB[i] = mMidle[i];
      }
      c = mObj->func(mMidle);
      for(int i = 0; i < mObj->getDim(); i ++) {
        FT l, a, b;
        
        l = 0.5 *(s.mB[i] - s.mA[i]);
        mLB[i] = s.mA[i];
        mUB[i] = s.mB[i];
        mObj->gradInt(mLB, mUB, mGradL, mGradU);
        BNBInterval<FT>::mult(-l, l, mGradL[i], mGradU[i], &a, &b);
        c += a;
      }
      return c;
    }
    
    FT evalPosyp(typename GOFactory<FT>::Set &s) 
    {
      FT c, e;
      e = getMinValue<FT>();
      for(int i = 0; i < mObj->getDim(); i ++) {
        mLB[i] = s.mA[i];
        mUB[i] = s.mB[i];
      }
      mObj->gradInt(s.mA, s.mB, mGradL, mGradU);
      for(int i = 0; i < mObj->getDim(); i ++) {
        FT l, a, b;
        
        l = 0.5 *(s.mB[i] - s.mA[i]);
        mLB[i] = mUB[i] = 0.5 * (s.mA[i] + s.mB[i]);
        mObj->funcMin(mLB, mUB, &c);
        mLB[i] = s.mA[i];
        mUB[i] = s.mB[i];
        BNBInterval<FT>::mult(-l, l, mGradL[i], mGradU[i], &a, &b);
        c += a;
        e = BNBMAX(e, c);
      }
      return e;
    }
    
    FT eval(typename GOFactory<FT>::Set &s)
    {
      FT estim;
      
      estim = getMinValue<FT>();      
      if(mEvalOptions & EvalOptions::EVAL_FUNCTION)
      {
        FT e;
        e = evalFunc(s);
        estim = BNBMAX(estim, e);
      }
      
      if(mEvalOptions & EvalOptions::EVAL_GRAD_SIMPLE)
      {
        FT e;
        e = evalGradSimple(s);
        estim = BNBMAX(estim, e);
      }
      
      if(mEvalOptions & EvalOptions::EVAL_FORM1)
      {
        FT e;
        e = evalForm1(s);
        estim = BNBMAX(estim, e);
      }
      
      if(mEvalOptions & EvalOptions::EVAL_HANSEN)
      {
        FT e;
        e = evalHansen(s);
        estim = BNBMAX(estim, e);
      }
      
      if(mEvalOptions & EvalOptions::EVAL_POSYP)
      {
        FT e;
        e = evalPosyp(s);
        estim = BNBMAX(estim, e);
      }

      return estim;
    }
    
            

    
    
    unsigned int mEvalOptions;
    unsigned int mReduceOptions;
    FT *mGradL;
    FT *mGradU;
    FT *mMidle;
    FT *mLB;
    FT *mUB;
    FT *mPoint;
    FT mEps;
    int *mMask;
    int *mBestMask;
    IntervalObjective<FT>* mObj;    
};

#endif
