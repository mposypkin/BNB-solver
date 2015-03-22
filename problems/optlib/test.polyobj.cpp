#include <stdio.h>
#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/optlib/conjgrad.hpp>
#include <problems/optlib/fletchsearch.hpp>
#include "polyobjective.hpp"

main()
{
  Polynom<double> poly;
  unsigned int deg1[3] = {2,2,0};
  Monom<double> mon1(3, 1, deg1);
  unsigned int deg2[3] = {0,2,2};
  Monom<double> mon2(3, 1, deg2);
  poly.addMonom(mon1);
  poly.addMonom(mon2);
  PolynomTxt fmt;
  std::string str = PolyUtil::toString(fmt, poly);
  PolyObjective<double> pobj(&poly);
  ConjGrad<double> cg;
  FletchSearch<double> ls;  
  cg.setLineSearch(&ls);
  cg.setObjective(&pobj);
  cg.setMaxIters(1000);
  cg.setEps(0.000001);
  double x[3] = {5, 4, 3};
  double v;
  cg.search(x, &v);
  if(BNBABS(v) > 0.0001) {
    printf("Search failed\n");
    return -1;
  }
  return 0;
}
