#ifndef __CONSSM_HPP__
#define __CONSSM_HPP__

#include <problems/cover/common/go.hpp>
#include <problems/optlib/objective.hpp>

class ConsSolutionMaker : public GOFactory<double>::SolutionMaker {
  
  public:
    /**
     * Base class for evaluating projections
     */
    class Projector {
      public:
        
        virtual void project(double* x, double* y) = 0;
    };
    
    ConsSolutionMaker()
    {
      mObj = NULL;
      mProj = NULL;
      mX = NULL;
      mDim = -1;
    }
    
    ~ConsSolutionMaker() 
    {
      if(mX)
        free(mX);
    }
    
    bool make(const  GOFactory<double>::Set& s,  GOFactory<double>::Solution& sol)
    {      
      BNB_ASSERT(mObj);
      BNB_ASSERT(mProj);
      int n = mObj->getDim();
      double* x = getX(n);
      for(int i = 0; i < n; i ++) {
        x[i] = 0.5 * (s.mA[i] + s.mB[i]);
      }
      mProj->project(x, (double*)sol.mX);
      sol.mFval = mObj->func((double*)sol.mX);
      /** TMP **/
      /*mObj->grad(sol.mX.getPtr(), x);
      double norm = VecUtils::vecNormTwo(n - 1, x);
      sol.mX[n - 1] = norm / 2.;*/
      return true;
    }
    
    
    void setObjective(Objective<double>* obj)
    {
      mObj = obj;
    }
    
    void setProjector(Projector* proj)
    {
      mProj = proj;
    }
    
  private:
    
    double* getX(int dim)
    {
      if(mDim != dim) {
        mDim = dim;
        mX = (double*) realloc(mX, mDim * sizeof(double));
        return mX;
      } else
        return mX;
    }
    
    Projector* mProj;
    Objective<double>* mObj;
    double *mX;
    int mDim;
};

#endif
