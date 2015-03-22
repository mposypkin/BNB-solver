#include <stdio.h>
#include "polynom.hpp"
#include "polynomtxt.hpp"
#include "polyutil.hpp"


main()
{
  Polynom<double> poly;
  PolynomTxt fmt;
  unsigned int deg1[3] = {2,1,3};
  Monom<double> mon1(3, 2, deg1);
  unsigned int deg2[3] = {1,1,1};
  Monom<double> mon2(3, -1, deg2);
  poly.addMonom(mon1);
  poly.addMonom(mon2);
  std::string str = PolyUtil::toString(fmt, poly);
  if(str != "2 x0^2 x1 x2^3 - x0 x1 x2") {
    printf("%s\n", str.c_str());
    return -1;
  }
  double vars[3] = {2,2,-2};
  if(poly.getValue(vars) != -120)
    return -1;
  double lvars[3] = {-1, -1, -1};
  double uvars[3] = {1, 1, 1};
  double l, u;
  poly.getBounds(lvars, uvars, &l, &u);
  if((l != -3) || (u != 3)) {
    printf("error computing polynom bounds: %lf, %lf\n", u, l);
    return -1;
  }
  poly.deriv(0);
  str = PolyUtil::toString(fmt, poly);
  if(str != "4 x0 x1 x2^3 - x1 x2") {
    printf("%s\n", str.c_str());
    return -1;
  }
  poly.deriv(0);
  str = PolyUtil::toString(fmt, poly);
  if(str != "4 x1 x2^3") {
    printf("%s\n", str.c_str());
    return -1;
  }
  return 0;
}
