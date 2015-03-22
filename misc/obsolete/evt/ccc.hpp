#ifndef __CCC_HPP__
#define __CCC_HPP__

#include <problems/multigo/multicovobj.hpp>


class Ccc : public multicovObjective {

  public:

  void func(double* x, double* y) {
    double xpy = x[0] + x[1];
    double xmy = x[0] - x[1];
    y[0] = 0.5 * (sqrt(1 + xpy * xpy) + sqrt(1 + xmy * xmy) + xmy);
    y[1] = 0.5 * (sqrt(1 + xpy * xpy) + sqrt(1 + xmy * xmy) - xmy);
  };

  int getNumOfParams()
  {
    return 2;
  }

  int getNumOfCriteria()
  {
    return 2;
  }

  void getLipConstants(double* x, double* l)
  {
    l[0] = 1;
    l[1] = 1;
  }
};

#endif
