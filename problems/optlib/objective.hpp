#ifndef __OBJECTIVE__
#define __OBJECTIVE__

#include <util/common/bnberrcheck.hpp>
#include "function.hpp"

/**
 * A Basic class for optimization target 
 * (defines function, its gradient, etc.)
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
template <typename FloatType> class Objective : public Function<FloatType>  {
public:
  
  /**
    * Objective kinds:
    */

  /**
    * Objective function
    */
  static const int OBJECTIVE = 1;

  /**
    * Equality constraint
    */
  static const int EQCONSTRAINT = 2;

  /**
    * Inequality constraint
    */
  static const int INEQCONSTRAINT = 3;

  /**
    * Set objective kind
    */
  void setKind(int kind) 
  {
    mKind = kind;
  }

  /**
    * Get objective kind
    */
  int getKind() const 
  {
    return mKind;
  }

  Objective()
  {
    mKind = OBJECTIVE;
    mDim = -1;
  }
  
  /**
   * Set number of dimensions
   *
   * @param dim number of space dimensions
   */
  void setDim(int dim)
  {
    mDim = dim;
  }
  

  /**
   * Gradient
   * 
   * @param x argument
   * @param g gradient
   */
  virtual void grad(const FloatType* x, FloatType* g)
  {
    BNB_ERROR_REPORT("Gradient not implemented");
  }
  
  /**
   * Gradient and function
   * (needed for faster evaluation in some methods)
   *
   * @param x argument
   * @param g gradient
   */
  virtual FloatType gradAndFunc(const FloatType* x, FloatType* g)
  {
      FloatType v = this->func(x);
      grad(x, g);
      return v;
    //BNB_ERROR_REPORT("Gradient and function not implemented");
  }
  
  /**
   * Calculate Hessian matrix
   * @param x point
   * @param H hessian ((n + 1) x n)
   */
  virtual void hess(const FloatType* x, FloatType* H)
  {
    BNB_ERROR_REPORT("Hessian not implemented");
  }
   
  /**
   * Calculate Hessian matrix
   * @param x point
   * @param H hessian 
   * @return dimension (may be less then n with possible constraints)
   */
  virtual int hessn(const FloatType* x, FloatType* H)
  {
    BNB_ERROR_REPORT("Hessian not implemented");
  }
  
  /**
   * Checks evaluation gradient accuracy
   * @param x point 
   * @param g gradient to check
   * @param eps expected eccuracy
   * @param delta step for grad estimate
   */
  /*void checkGrad(FloatType* x, FloatType*  g, FloatType eps, FloatType delta)
  {
    FloatType * xn, v1, v2, der;
    BNB_ASSERT(mDim != -1);
    v1 = func(x);
    xn = (FloatType*) malloc(mDim * sizeof(FloatType));
    for(int i = 0; i < mDim; i ++) 
      xn[i] = x[i];
    for(int i = 0; i < mDim; i ++) {
      xn[i] = x[i] + delta;
      v2 = func(xn);
      der = (v2 - v1) / delta;
      xn[i] = x[i];
    }
    free(xn);
  }*/
  
  int getDim()
  {
    BNB_ASSERT(mDim != -1);
    return mDim;
  }


protected:

  int mDim;
  int mKind;
};

#endif
