#ifndef __NEIGH_HPP__
#define __NEIGH_HPP__

#include <stdio.h>
#include <stdlib.h>
#include "locoptim.hpp"
#include <util/common/vec.hpp>

template <typename FT> class Neigh : public LocalOptimizer<FT> {
  public:
    /**
     * Constructor
     */
    Neigh()
    {
      mMaxIters = -1;
      mDrops = 10;
      mMaxAttempts = 1;
      mG = NULL;
      mX = NULL;
      mY = NULL;
    }
    
        
    /**
     * Setup objective
     * @param obj objective
     */
    void setObjective(Objective<FT>* obj)
    {
      LocalOptimizer<FT>::setObjective(obj);
      mG = (FT*)realloc(mG, LocalOptimizer<FT>::mObj->getDim() * sizeof(FT));
      mX = (FT*)realloc(mX, LocalOptimizer<FT>::mObj->getDim() * sizeof(FT));
      mY = (FT*)realloc(mY, LocalOptimizer<FT>::mObj->getDim() * sizeof(FT));
    }
    
    
    /**
     * Destructor
     */
    ~Neigh()
    {
      if(mG)
        free(mG);
      if(mX)
        free(mX);
      if(mY)
        free(mY);
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
     * Setup drops counter
     * @param drops drops counter
     */
    void setDrops(int drops)
    {
      mDrops = drops;
    }
    
    /**
     * Set up number of attempts
     * @param attempts number of attempts
     */
    void setMaxAttempts(int attempts)
    {
      mMaxAttempts = attempts;
    }
    
   /**
     * Perform search
     * @param x start point and result
     * @param v pointer to the resulting value
     * @return true if search converged and false otherwise
     */
    bool search(FT* x, FT* v)
    {
      int iters, n, attempts;
      iters = 0;
      attempts = 0;
      n = LocalOptimizer<FT>::mObj->getDim();
      do {
        FT u, uold, norm;
        u = LocalOptimizer<FT>::mObj->func(x);
        uold = u;
        LocalOptimizer<FT>::mObj->grad(x, mG);
        norm = VecUtils::vecNormOne(n, mG);
        mDrops = (10. / norm) + 1;        
        mMu = BNBMAX(0.01, .01 / norm);
        for(int i = 0; i < mDrops; i ++) {
          FT z;
          generate(x);
          z = LocalOptimizer<FT>::mObj->func(mX);
          if(z < u) {
            u = z;
            VecUtils::vecCopy(n, mX, mY);
          }
        }
        if(u < uold) {
          printf("Iters = %d, drops = %d, norm = %lf, mu = %lf, attempts = %d: improved from %lf to %lf\n", iters, mDrops, norm, mMu, attempts, uold, u);
          VecUtils::vecSaxpy(n, mY, x, -1., mG);
          VecUtils::vecCopy(n, mY, x);
          
          attempts = 0;
        } else {          
          if(norm < mEps) {
            printf("Iters = %d, attempts = %d, norm = %lf, val = %lf \n", iters, attempts, norm, u);
            *v = u;
            return true;
          } else if(attempts >= mMaxAttempts) {
            FT a, step, s,  sold;
            a = 0;
            step = .0001;
            s = sold = LocalOptimizer<FT>::mObj->func(x);
            do {
              FT gamma;
              sold = BNBMIN(s, sold);
              gamma = (a + step) / norm;
              VecUtils::vecSaxpy(n, x, mG, -gamma, mX);
              s = LocalOptimizer<FT>::mObj->func(mX);
              if(s < sold) {
                a += step;
              } else {
                step *= 0.5;
              }
            } while((BNBABS(sold - s) > 0.0001) || (s > sold));
            if(iters % 1000 == 0)
              printf("Grad=>Iters = %d, attempts = %d, norm = %lf, val = %lf, step = %lf \n", iters, attempts, norm, s, step);
            VecUtils::vecCopy(n, mX, x);
            attempts = 0;
            /*
            *v = s;
            return true;
            */
          } else
            attempts ++;
        }
      } while((mMaxIters == -1) || (iters ++ < mMaxIters));
      return false;
    }
    
    
  private:
    
    void generate(FT* x)
    {
      int n;
      n = LocalOptimizer<FT>::mObj->getDim();
      for(int i = 0; i < n; i ++) {
        FT alpha;
        alpha = 1. - (2. * (double)rand()) / ((double)RAND_MAX);
        mX[i] = x[i] + alpha * mMu;
      }
    }
           
    FT mMu;
    FT mEps;
    FT *mG;
    FT *mX;
    FT *mY;
    int mMaxIters;
    int mDrops;
    int mMaxAttempts;
};
#endif
