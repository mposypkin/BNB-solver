#ifndef __EVT_HPP__
#define __EVT_HPP__

#include <util/common/util.hpp>
#include <problems/multigo/multicovobj.hpp>


class Evt : public multicovObjective {

  public:

  void func(double* x, double* y) {
    y[0] = x[0];
    y[1] = BNBMIN(BNBABS(1 - x[0]), 1.5 - x[0]);
  };

  void grad(double* x, double* g)
  {
    g[0] = 1.;
    if((x[0] >= 1.) && (x[0] < 1.25))
      g[1] = 1.;
    else
      g[1] = -1.;
  }

  int getNumOfParams()
  {
    return 1;
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

  void getGradLipConstants(double* x, double* l)
  {
    l[0] = 0;
    l[1] = 0;
  }

};

#endif
