#include <stdio.h>
#include "monom.hpp"
#include "polynomtxt.hpp"
#include "polyutil.hpp"

main()
{
  unsigned int deg[3] = {2,1,3};
  PolynomTxt fmt;
  Monom<double> mon(3, -2, deg);
  std::string txt = PolyUtil::toString(fmt, mon);
  if(txt != "2 x0^2 x1 x2^3") {
    printf("%s\n", txt.c_str());
    return -1;
  } 
  double vars[3] = {2, 2, -2};
  if(mon.getValue(vars) != 128) {
    return -1;
  }
  mon.deriv(2);
  std::string ntxt = PolyUtil::toString(fmt, mon);
  if(ntxt != "6 x0^2 x1 x2^2") {
    printf("%s\n", ntxt.c_str());
    return -1;
  }
  mon.deriv(1);
  std::string nntxt = PolyUtil::toString(fmt, mon);;
  if(nntxt != "6 x0^2 x2^2") {
    printf("%s\n", nntxt.c_str());
    return -1;
  }
  double lb[3] = {-1, 0, -1};
  double ub[3] = {1, 0, 1};
  double l, u;
  mon.getBounds(lb, ub, &l, &u);
  if((l != -6) || (u != 0)) {
    printf("Error computing interval bounds: %lf, %lf\n", l, u);
    return -1;
  }
  mon.deriv(1);
  std::string nnntxt = PolyUtil::toString(fmt, mon);;
  if(nnntxt != "") {
    printf("%s\n", nnntxt.c_str());
    return -1;
  }

  return 0;
}
