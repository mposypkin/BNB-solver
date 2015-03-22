#ifndef __MULTIOBJECTIVE_HPP__
#define __MULTIOBJECTIVE_HPP__

#include "multifunc.hpp"

/**
  * The objective for multicriteria optimization
  */

class MultiObjective : public MultiFunction {

  public:

    /**
      * Return the number of function parameters
      */
    virtual int getNumOfParams() = 0;

    /**
      * Return the number of criteria
      */
    virtual int getNumOfCriteria() = 0;
};

#endif
