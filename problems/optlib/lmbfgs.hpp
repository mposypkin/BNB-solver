#ifndef __LMBFGS_HPP__
#define __LMBFGS_HPP__

#include "locoptim.hpp"
#include "lbfgs.h"

extern "C" double lbfgsEvaluate(void *instance, const double *x, double *g, const int n,const lbfgsfloatval_t step);


/**
 * Limited memory BFGS
 */

class LMBFGS : public LocalOptimizer <double>{
  public:
    LMBFGS()
    {
      lbfgs_parameter_init(&mParam);
//       mParam.linesearch = 1;
    }
  /**
   * Perform search
   * @param x start point and result
   * @param v 
   * @return true if search converged and false otherwise
   */
    bool search(double* x, double* v) 
    {
      int rc;
      mObjective = LocalOptimizer <double>::mObj;
      int n = LocalOptimizer <double>::mObj->getDim();
      rc = lbfgs(n, x, v, lbfgsEvaluate, NULL, NULL, &mParam);
      return true;
    }
    
    /**
     * Install epsilon
     * @param eps 
     */
    void setEps(double eps)
    {
      mParam.epsilon = eps;
    }

    
    /**
     * Setup update number
     */
    void setUpdate(int upd)
    {
      mParam.m = upd;
    }
    
    static Objective<double>* mObjective;
    
  private:
    lbfgs_parameter_t mParam;
    

};

#endif
