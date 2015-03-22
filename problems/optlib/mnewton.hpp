#ifndef __MNEWTON__HPP_
#define __MNEWTON__HPP_

#include "newton.hpp"

template <typename FT> class MNewton :  public LocalOptimizer<FT> {
  
  public:
    MNewton()
    {
      mG = NULL;
      mNewt = new Newton<FT>();
    }
    

  /**
   * Install epsilon
   * @param eps 
   */
    void setEps(FT eps)
    {
      mNewt->setEps(eps);
      mEps = eps;
    }
    
  /**
     * Install maximal iters counter
     * @param iters iters counter
   */
    void setMaxIters(int iters)
    {
      mMaxIters = iters;
    }
  
  /**
     * Setup objective
     * @param obj objective
   */
    void setObjective(Objective<FT>* obj)
    {
      LocalOptimizer<FT>::setObjective(obj);
      mG = (FT*)realloc(mG, LocalOptimizer<FT>::mObj->getDim() * sizeof(FT));
      BNB_ASSERT(mG);
      mNewt->setObjective(obj);
    }

    ~MNewton()
    {
      free(mG);
      delete mNewt;
    }
    
    /**
     * Perform search
     * @param x start point and result
     * @param v objective
     * @return true if search converged and false otherwise
     */
    bool search(FT* x, FT* v)
    {
      int n, i;
      FT vv, step, vbest, norm;

      n = LocalOptimizer<FT>::mObj->getDim();
      vbest = LocalOptimizer<FT>::mObj->func(x);
      step = 1.;
      i = 0;
      do{
        LocalOptimizer<FT>::mObj->grad(x, mG);
        norm = VecUtils::vecNormOne(n, mG);          
        if((norm <= mEps) || (i > mMaxIters))
          break;
        for(int j = 0; j < n; j ++)
          x[j] -= step * mG[j]/norm;
        vv = LocalOptimizer<FT>::mObj->func(x);
        if((i % 10000) == 0)
          printf("i = %d, vbest = %lf, vv = %lf (norm = %lf), step = %lf\n", i, vbest, vv, norm, step);
        if(vv > vbest) {
          for(int j = 0; j < n; j ++)
            x[j] += step * mG[j]/norm;
          step *= 0.5;
        } else {
          vbest = vv;
          step *= 1.01;
        }
        i ++;
      } while(1);
      if(norm > mEps)
        mNewt->search(x, v);
      else
        *v = LocalOptimizer<FT>::mObj->func(x);
      printf("i = %d, norm = %lf, v = %lf\n", i, norm, *v);
      return true;
    }

    
  private:
    FT* mG;    
    Newton<FT>* mNewt;
    int mMaxIters;
    FT mEps;
    
};
#endif
