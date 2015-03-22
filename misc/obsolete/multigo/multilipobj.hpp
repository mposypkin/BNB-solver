#ifndef __multicovOBJ_HPP__
#define __multicovOBJ_HPP__

#include "multiobjective.hpp"
#include <util/common/util.hpp>

class multicovObjective : public MultiObjective {
  public:
  /**
    * Retrieve the vector of Lipzic constants
    * @param x the coordinate vector
    * @param l the lipzic constant vector
    */
  virtual void getLipConstants(double* x, double* l)
  {
    BNB_ERROR_REPORT("Not implemented");
  }

  /**
    * Retrieve the vector of Lipzic constants for gradient
    * @param x the coordinate vector
    * @param l the lipzic constant vector
    */
  virtual void getGradLipConstants(double* x, double* l)
  {
    BNB_ERROR_REPORT("Not implemented");
  }
};


#endif
