#ifndef __STEEPDESC__HPP_
#define __STEEPDESC__HPP_

#include "locoptim.hpp"
#include "linesearch.hpp"

template <typename FT> class SteepDesc : public LocalOptimizer<FT>
{
  public: 
    
   /**
   * Constructor
    */
    SteepDesc()
    {
      mMaxIters = -1;
      mG = NULL;
      mLs = NULL;
    }
    /**
     * Destructor
     */
    ~SteepDesc()
    {
      if(mG)
        free(mG);
    }

        
    /**
     * Setup objective
     * @param obj objective
     */
    void setObjective(Objective<FT>* obj)
    {
      LocalOptimizer<FT>::setObjective(obj);
      BNB_ASSERT(mLs);
      mLs->setObjective(obj);
      mG = (FT*)realloc(mG, obj->getDim() * sizeof(FT));
    }
    
    
    /**
     * Install epsilon
     * @param eps 
     */
    void setEps(FT eps)
    {
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
     * Set up line search algorithm
     * @param ls class implementing line search
     */
    void setLineSearch(LineSearch<FT>* ls)
    {
      mLs = ls;
    }
    
    virtual bool search(FT* x, FT* v)
    {
      int n, i;
      FT norm = 111.;
      n = LocalOptimizer<FT>::mObj->getDim();
      i = 0;
      do {
        FT alpha;
        i ++;
        LocalOptimizer<FT>::mObj->grad(x, mG);
        norm = VecUtils::vecNormTwoSqr(n, mG);
        printf("norm = %lf\n", norm);
        fflush(stdout);
        if(norm <= mEps)
          break;
        if(i > mMaxIters) {
          return false;
        }
        VecUtils::revert(n, mG);
        alpha = mLs->search(x, mG);
        VecUtils::vecSaxpy(n, x, mG, alpha, x);
      } while(1);
      *v = LocalOptimizer<FT>::mObj->func(x);
      return true;
    }
    
  private:
    LineSearch<FT> *mLs;
    FT* mG;
    FT mEps;
    int mMaxIters;
};

#endif
