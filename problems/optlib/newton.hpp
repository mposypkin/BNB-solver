#ifndef _NEWTON_HPP_
#define _NEWTON_HPP_
#include <util/linal/gauss.hpp>
#include <util/linal/jacobi.hpp>

template <typename FT> class Newton  : public LocalOptimizer<FT> {
  
  public:
    
  Newton()
  {
    mG = NULL;
    mH = NULL;
    mP = NULL;
    mA = NULL;
    mMaxIters = -1;
    mMu = 1.;
  }
  
  ~Newton()
  {
    if(mG)
      free(mG);
    if(mH)
      free(mH);
    if(mP)
      free(mP);
    if(mA)
      free(mA);
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
    mP = (FT*)realloc(mP, n * sizeof(FT));
    BNB_ASSERT(mP);
    mH = (FT*)realloc(mH, n * (n + 1) * sizeof(FT));
    BNB_ASSERT(mH);    
    mA = (FT*)realloc(mA, n * n * sizeof(FT));
    BNB_ASSERT(mA);    
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
    
    n = LocalOptimizer<FT>::mObj->getDim();
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
      LocalOptimizer<FT>::mObj->grad(x, mG);
      norm = VecUtils::vecNormOne(n, mG);        
      
  /*      for(int i = 0; i < n; i ++)
          printf("before: g[%d] = %lf\n", i, mG[i]);  */
      
      if(norm <= mEps) {
        *v = LocalOptimizer<FT>::mObj->func(x);
        printf("Iters: %d\n", iters);
        return true;
      }
      LocalOptimizer<FT>::mObj->hess(x, mH);
      for(int i = 0; i < n; i ++) 
        mH[i * (n + 1) + n] = -mMu * mG[i];
      
      /*
      for(int i = 0; i < n; i ++)
        for(int j = 0; j < (n + 1); j ++)
          printf("mH[%d][%d] = %lf\n", i, j, mH[i * (n + 1) + j]);

      check(n, 0.0001, x, mA);
      for(int i = 0; i < n; i ++) {
        for(int j = 0; j < n; j ++)  {
          printf("%lf(%lf) ", mH[i * (n + 1) + j], mA[i * n + j]);
          //mH[i * (n + 1) + j] = mA[i * n + j];
        }
        printf("\n");
      }
      printf("\n");
      */
      FT M;
      /*
      M = 0;
      for(int i = 0; i < n; i ++) {
        for(int j = 0; j < n; j ++) {
          FT h;
          h = BNBABS(mH[i * (n + 1) + j]);
          if(h > M)
            M = h;
        }
      }
      */
      FT *A, *V, *d;
      A = (FT*)malloc(n * n * sizeof(FT));
      V = (FT*)malloc(n * n * sizeof(FT));
      d = (FT*)malloc(n * sizeof(FT));
      for(int i = 0; i < n; i ++)
        for(int j = 0; j < n; j ++) 
          A[i * n + j] = mH[i * (n + 1) + j];
      Jacobi<FT> jac;
      jac.solve(n, A, d, V);
      jac.sort(n, d, V);
      M = -BNBMIN(0, d[n - 1]);
      printf("eigen: ");
      VecUtils::vecPrint(n, d);
      free(d);
      free(V);
      free(A);
      //M = 100.;
      printf("M = %lf\n", M);
      for(int i = 0; i < n; i ++)
        mH[i * (n + 1) + i] += 1.5 * M;
      Gauss<FT> gauss;
      gauss.solve(n, mH, mP);
      
      /*
      for(int i = 0; i < n; i ++)
        printf("p[%d] = %lf\n", i, mP[i]);  

      for(int i = 0; i < n; i ++) {
        FT a;
        a = 0;
        for(int j = 0; j < n; j ++ ) {
          a += mA[i * n + j] * mP[j];
        }
        printf("(%lf vs %lf), %lf\n", a, mG[i], mMu * mG[i] + a);
      }
      */
      /*
      for(int i = 0; i < n; i ++)
        printf("p[%d] = %lf, x[%d] = %lf\n", i, mG[i], i, x[i]);
      */
      for(int i = 0; i < n; i ++) 
        x[i] += mP[i];
      iters ++;
    }        
  }
   
  private:
    
    void check(int n, FT delta, FT* x, FT* h)
    {
      
      FT g[1024], g1[1024], y[1024];
      LocalOptimizer<FT>::mObj->grad(x, g);
      for(int i = 0; i < n; i ++) {
        for(int k = 0; k < n; k ++) {
          if(k == i)
            y[k] = x[k] + delta;
          else
            y[k] = x[k];
        }
        LocalOptimizer<FT>::mObj->grad(y, g1);
        for(int j = 0; j < n; j ++) 
          h[i * n + j] = (g1[j] - g[j]) /delta;
      }
    }
    
    FT* mG;
    FT* mP;
    FT* mH;
    FT* mA;
    FT mMu;
    FT mEps;
    FT mMaxIters;
};
#endif
