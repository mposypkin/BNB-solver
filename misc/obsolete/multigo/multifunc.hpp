#ifndef __MULTIFUNC_HPP__
#define __MULTIFUNC_HPP__

#include <util/common/bnberrcheck.hpp>

class MultiFunction {
  public:

    /**
      * Evaluates the function in a given parameter point and updates the respective 
      * value coordinates.
      *
      * @param x pointer to the  paramter array
      * @param y pointer to the values array
      */
    virtual void func(double* x, double* y) = 0;


    /**
      * Evaluates the function gradients (one per criteria) in a given point
      *
      * @param x point the parameter array
      * @param y pointe to hold gradient matrix
      */
    virtual void grad(double* x, double* g)
    {
      BNB_ERROR_REPORT("Not implemented");
    }
};

#endif


