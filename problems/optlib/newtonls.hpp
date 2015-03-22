#ifndef __NEWTONLS_HPP___
#define __NEWTONLS_HPP___


#include <util/linal/gauss.hpp>
#include "linesearch.hpp"
#include <util/linal/jacobi.hpp>

template <typename FT> class NewtonLS  : public LocalOptimizer<FT> {
  
  public:
    
    NewtonLS()
    {
      mG = NULL;
      mH = NULL;
      mDir = NULL;
      mLs = NULL;
      mMaxIters = -1;
      mMu = 1.;
    }
  
    ~NewtonLS()
    {
      if(mG)
        free(mG);
      if(mH)
        free(mH);
      if(mDir)
        free(mDir);
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
     * Setup objective
     * @param obj objective
   */
    void setObjective(Objective<FT>* obj)
    {
        LocalOptimizer<FT>::setObjective(obj);
        int n = LocalOptimizer<FT>::mObj->getDim();
        mG = (FT*)realloc(mG, n * sizeof(FT));
        BNB_ASSERT(mG);
        mDir = (FT*)realloc(mDir, n * sizeof(FT));
        BNB_ASSERT(mDir);
        mH = (FT*)realloc(mH, n * (n + 1) * sizeof(FT));
        BNB_ASSERT(mH);
        if(mLs)
            mLs->setObjective(LocalOptimizer<FT>::mObj);
    }
    /**
     * Set up line search algorithm
     * @param ls class implementing line search
     */
    void setLineSearch(LineSearch<FT>* ls)
    {
      mLs = ls;
    }

  /**
     * Perform search
     * @param x start point and result
     * @param v objective
     * @return true if search converged and false otherwise
   */
    bool search(FT* x, FT* v)
    {
      int n, iters;
    
      n = mObj->getDim();
      iters = 0;
      for(;;){
        FT norm;
      
      /*
        for(int i = 0; i < n; i ++)
        printf("before: x[%d] = %lf\n", i, x[i]);
      */
        if((mMaxIters != -1) && (iters > mMaxIters)){
          printf("Iters: %d\n", iters);
          return false;
        }
        mObj->grad(x, mG);
        norm = VecUtils::vecNormTwoSqr(n, mG);        
      /*
        for(int i = 0; i < n; i ++)
        printf("before: g[%d] = %lf\n", i, mG[i]);  
      */
        if(norm <= mEps) {
          *v = mObj->func(x);
          printf("Iters: %d\n", iters);
          return true;
        }
        mObj->hess(x, mH);
        
        for(int i = 0; i < n; i ++) 
          mH[i * (n + 1) + n] = -mMu * mG[i];
        FT M, *A, *V, *d, *c;
        A = (FT*)malloc(n * n * sizeof(FT));
        V = (FT*)malloc(n * n * sizeof(FT));
        d = (FT*)malloc(n * sizeof(FT));
        c = (FT*)malloc(n * sizeof(FT));
        for(int i = 0; i < n; i ++)
          for(int j = 0; j < n; j ++) 
            A[i * n + j] = mH[i * (n + 1) + j];
        Jacobi<FT> jac;
        jac.solve(n, A, d, V);
        
        for(int i = 0; i < n; i ++) {
         if(d[i] < 0.) {
           printf("---!!!%lf", d[i]);
           d[i] = BNBMAX(0.1, -d[i]);
           printf("---%lf\n", d[i]);
          }
       }
        for(int i = 0; i < n; i ++) 
          c[i] = -mMu * mG[i];
         jac.solveSys(n, d, V, c, mDir); 
/*        jac.sort(n, d, V);
        M = -BNBMIN(0, d[n - 1]);
        printf("eigen: ");
        VecUtils::vecPrint(n, d);*/
        free(c);
        free(d);
        free(V);
        free(A);
      //M = 100.;
//         printf("M = %lf\n", M);
//         for(int i = 0; i < n; i ++)
//           mH[i * (n + 1) + i] += 1.5 * M;

      
      /*
        for(int i = 0; i < n; i ++)
        for(int j = 0; j < (n + 1); j ++)
        printf("mH[%d][%d] = %lf\n", i, j, mH[i * (n + 1) + j]);
      */
/*        Gauss<FT> gauss;
        if(gauss.solve(n, mH, mDir) == false) {
          VecUtils::vecCopy(n, mG, mDir);
        }*/
      
      /*
        for(int i = 0; i < n; i ++)
        printf("p[%d] = %lf, x[%d] = %lf\n", i, mG[i], i, x[i]);
      */
        if(mLs) {
          FT sp, alpha;
          sp = VecUtils::vecScalarMult(n, mG, mDir);
          printf("sp = %lf\n", sp);
          
          if(sp < 0) {
            for(int i = 0; i < n; i ++)
              mDir[i] = -mDir[i];
          }
          
          {
//             printf("searching...");
            alpha = mLs->search(x, mDir);
            printf("alpha = %lf\n", alpha);
          } 
          VecUtils::vecSaxpy(n, x, mDir, -alpha, x);
        } else {
          for(int i = 0; i < n; i ++) 
            x[i] += mDir[i];
        }
        iters ++;
      }        
    }
   
  private:
    
    FT* mG;
    FT* mH;
    FT* mDir;
    Objective<FT> *mObj;    
    FT mMu;
    FT mEps;
    FT mMaxIters;
    LineSearch<FT> *mLs;

};
#endif
