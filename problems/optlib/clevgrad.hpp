#ifndef __CLEVGRAD__
#define __CLEVGRAD__

#include "projector.hpp"
#include "userhandler.hpp"
#include "locoptim.hpp"

template <typename FT> class ClevGrad : public LocalOptimizer<FT>{
  public:
    /**
     * Constructor
     */
    ClevGrad()
    {
      mMaxIters = -1;
      mG = NULL;
      mX = NULL;
      mProjector = NULL;
      mUserHandler = NULL;
    }
    
    /**
     * Retrieve objective
     * @return objective
    Objective<FT>* getObjective()
    {
      return mObj;
    }
     * /
        
    /**
     * Setup objective
     * @param obj objective
     */
    void setObjective(Objective<FT>* obj)
    {
      LocalOptimizer<FT>::mObj = obj;
      mX = (FT*)realloc(mX, LocalOptimizer<FT>::mObj->getDim() * sizeof(FT));
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
      int n, i;
      int attempts;
      FT vbest;
      FT step = 1;
      i = 0;
      vbest = LocalOptimizer<FT>::mObj->func(x);
      n = LocalOptimizer<FT>::mObj->getDim();
      attempts = 0;
      if(mProjector)
        mProjector->project(n, x);
      for(i = 0; (mMaxIters == -1) ? true : i < mMaxIters; i ++) {
        FT norm, vv;
        LocalOptimizer<FT>::mObj->grad(x, mG);
        if(mProjector)
          mProjector->project(n, mG);
        norm = VecUtils::vecNormTwo(n, mG);          
        if(norm < mEps) {
          printf("iters = %d, attempt = %d\n", i, attempts);
          if(attempts > -1) {
            *v = vbest;
            return true;
          } else {
            attempts ++;
            step = attempts * 0.1;
          }            
        }
        /*
        {
          FT step0, beststep;
          beststep = step;
          for(int m = 1; m <=100; m ++) {
            step0 = (100./((double)m)) * step;
            for(int j = 0; j < n; j ++)
              mX[j] = x[j] - step0 * mG[j]/norm;
            vv = mObj->func(mX);
            //printf("%lf ", vv);
            if(vv < vbest) {
              beststep = step0;
              vbest = vv;
            }
          }
          step = beststep;  
          //printf("\n");
        }
        */
        for(int j = 0; j < n; j ++)
          mX[j] = x[j] - step * mG[j]/norm;
        vv = LocalOptimizer<FT>::mObj->func(mX);
        if(((i % 10000) == 0) && (i != 0))  {
          printf("i = %d, vbest = %lf, vv = %lf (norm = %lf), step = %lf\n", i, vbest, vv, norm, step);
          /*
          for(int o = 0; o < 200; o ++) {
            FT* aux, delt, vvv;
            aux = (FT*) malloc(n * sizeof(FT));
            delt = (o * step) / 100.;
            for(int j = 0; j < n; j ++)
              aux[j] = x[j] -  delt * mG[j]/norm;
            vvv = mObj->func(aux);
            printf("%4.12lf %4.12lf\n", delt, vvv);
            free(aux);            
          }
          */
          if(mUserHandler)
            mUserHandler->debug(mX, mG);
        }

        if(vv > vbest) {
          step *= .5;
        } else {
          attempts = 0;
          step *= 1.01;
          for(int j = 0; j < n; j ++)
            x[j] = mX[j];
          vbest = vv;
        }
      }  
      printf("iters = %d\n", i);
      *v = vbest;
      return false;    
    }
    
    
    ~ClevGrad()
    {
      if(mG)
        free(mG);
      if(mX)
        free(mX);
    }
    
    /**
     * Setup projector pointer
     * @param proj projector pointer
     */
    void setProjector(Projector<FT> * proj)
    {
      mProjector = proj;
    }
    
    /**
     * Set up user handler
     * @param uh user handler
     */
    void setUserHandler(UserHandler<FT>* uh)
    {
      mUserHandler = uh;
    }
    
  private:
    FT mMu;
    FT mEps;
    FT mMaxIters;
    FT *mG;       
    FT *mX;
    Projector<FT>* mProjector;
    UserHandler<FT> *mUserHandler;
};
#endif
