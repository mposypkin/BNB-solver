#ifndef __SIMP_GRAD_HPP
#define __SIMP_GRAD_HPP

#include <stdio.h>
#include <stdlib.h>
#include "locoptim.hpp"
#include <util/common/vec.hpp>
#include <util/common/util.hpp>

template <typename FT> class SimpGrad : public LocalOptimizer<FT>{
  public:
    /**
     * Constructor
     */
    SimpGrad()
    {
      mMaxIters = -1;
      mG = NULL;
      mMu = 1;
    }
    
    /**
     * Setup objective
     * @param obj objective
     */
    void setObjective(Objective<FT>* obj)
    {
      LocalOptimizer<FT>::setObjective(obj);
      mG = (FT*)realloc(mG, LocalOptimizer<FT>::mObj->getDim() * sizeof(FT));
    }
    
    /**
     * Install multiplier
     * @param mu 
     */
    void setMu(FT mu)
    {
      mMu = mu;
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
     * Perform search
     * @param x start point and result
     * @param v pointer to the resulting value
     * @return true if search converged and false otherwise
    */
    bool search(FT* x, FT* v)
    {
      int iters = 0;
      int dim;
      FT xold[2048];
      FT norm, normp, w, wold;
      norm = getMaxValue<double>();
      
      dim = LocalOptimizer<FT>::mObj->getDim();
      w = LocalOptimizer<FT>::mObj->func(x);
      for(;;){
        if((mMaxIters != -1) && (iters > mMaxIters)){
          printf("Iters: %d (eps = %lf)\n", iters, mEps);
          return false;
        }
        wold = w;
        w = LocalOptimizer<FT>::mObj->func(x);
        LocalOptimizer<FT>::mObj->grad(x, mG);
        normp = norm;
        norm = VecUtils::vecNormTwoSqr(dim, mG);          
//         VecUtils::vecPrint(dim, x);
//         VecUtils::vecPrint(dim, mG);
/*         printf("(%lf), %lf, %lf ----\n", w, norm, normp);
         printf("Enter Mu:\n");
         scanf("%lf", &mMu);*/
         if(mMu < 0) {
           *v = LocalOptimizer<FT>::mObj->func(x);
           return true;
         }
        

        if(norm <= mEps) {
          *v = LocalOptimizer<FT>::mObj->func(x);
          printf("Iters: %d (eps = %lf), v = %lf\n", iters, mEps, *v);
          return true;
        } /*else if(wold <= w) {
          if(mMu > 0.0001) {
           mMu = mMu * 0.5;
           printf("MU = %lf\n", mMu);
           for(int i = 0; i < dim; i ++) 
             x[i] = xold[i];
           mObj->grad(x, mG);
          } else {
            *v = mObj->func(x);
            printf("Iters: %d (eps = %lf, mMu = %lf)\n", iters, mEps, mMu);
            return true;
          }          
        }*/
        for(int i = 0; i < dim; i ++) {
          xold[i] = x[i];
          x[i] = x[i] - (mMu * mG[i])  / norm;
        }
        iters ++;
      }        
    }
    ~SimpGrad()
    {
      if(mG)
        free(mG);
    }
  private:
    FT mMu;
    FT mEps;
    FT mMaxIters;
    FT *mG;
       
    
};
#endif
