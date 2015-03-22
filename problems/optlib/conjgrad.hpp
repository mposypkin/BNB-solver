
#ifndef __CONJGRAD__HPP_
#define __CONJGRAD__HPP_

#include "locoptim.hpp"
#include "linesearch.hpp"
#include <util/common/vec.hpp>

template <typename FT> class ConjGrad : public LocalOptimizer<FT>
{
  public: 
    
   /**
   * Constructor
    */
    ConjGrad()
    {
      mMaxIters = -1;
      mG = NULL;
      mG1 = NULL;
      mS = NULL;
      mS1 = NULL;
      mLs = NULL;
    }
    /**
     * Destructor
     */
    ~ConjGrad()
    {
      if(mG)
        free(mG);
      if(mG1)
        free(mG1);
      if(mS)
        free(mS);
      if(mS1)
        free(mS1);
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
      int n =  LocalOptimizer<FT>::mObj->getDim();
      mG = (FT*)realloc(mG, n * sizeof(FT));
      mG1 = (FT*)realloc(mG1, n * sizeof(FT));
      mS = (FT*)realloc(mS, n * sizeof(FT));
      mS1 = (FT*)realloc(mS1, n * sizeof(FT));
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
      n =  LocalOptimizer<FT>::mObj->getDim();
      i = 0;
      
     do {
        FT norm, alpha, beta;
         LocalOptimizer<FT>::mObj->grad(x, mG1);
//         mObj->checkGrad(x, mG1, 0.001, 0.0001);
        norm = VecUtils::vecNormTwoSqr(n, mG1);
        
        if(norm <= mEps)
          break;
        if(i > mMaxIters) {
          return false;
        }
        if((i % n) == 0)
          beta = 0.;
        else {
          FT a1, a2, a;
          a2 = 0;
          
          a = VecUtils::vecScalarMult(n, mG, mG);
          a1 = VecUtils::vecScalarMult(n, mG1, mG1);
          a2 = VecUtils::vecScalarMult(n, mG, mG1);
          beta = (a1 - a2) / a;
//          beta =  a1/a;
        }
        VecUtils::revert(n, mG1);
        VecUtils::vecSaxpy(n, mG1, mS, beta, mS1);
        alpha = mLs->search(x, mS1);
        //printf("alpha = %lf, f = %lf\n", alpha, mObj->func(x));
        VecUtils::vecSaxpy(n, x, mS1, alpha, x);
        VecUtils::vecCopy(n, mG1, mG);
        VecUtils::revert(n, mG);
        VecUtils::vecCopy(n, mS1, mS);
        i ++;
      } while(1);
      *v =  LocalOptimizer<FT>::mObj->func(x);
      return true;
    }
    
  private:
    LineSearch<FT> *mLs;
    FT* mG;
    FT* mG1;
    FT* mS;
    FT* mS1;    
    FT mEps;
    int mMaxIters;    
};

#endif
