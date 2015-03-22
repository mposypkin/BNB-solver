#ifndef __EVTM_HPP__
#define __EVTM_HPP__

#include <util/common/util.hpp>
#include <problems/multigo/multicovobj.hpp>


class Evtm : public multicovObjective {

  public:

  void func(double* x, double* y) {
    y[0] = x[0];
    y[1] = BNBMIN(x[1] + BNBABS(1 - x[0]), x[1] + 1.5 - x[0]);
  };

  void grad(double* x, double* g)
  {
    g[0] = 1.;
    g[1] = 0.;
    if((x[0] >= 1.) && (x[0] < 1.25))
      g[2] = 1.;
    else
      g[2] = -1.;
    g[3] = 1.;
  }

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

  void getGradLipConstants(double* x, double* l)
  {
    l[0] = 0;
    l[1] = 0;
  }

};

#endif
