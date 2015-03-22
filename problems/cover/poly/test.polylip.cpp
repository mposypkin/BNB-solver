#include <stdio.h>
#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/optlib/conjgrad.hpp>
#include <problems/optlib/fletchsearch.hpp>
#include <common/go.hpp>
#include <common/decomposer.hpp>
#include <common/segvecdecomp.hpp>
#include <common/lipzfdiscarder.hpp>
#include <common/lipzgdiscarder.hpp>
#include <kernel/bnblib/seq.hpp>
#include "polylipobj.hpp"


#define n 2

main()
{
  Polynom<double> poly;
  unsigned int deg1[n] = {2,2};
  Monom<double> mon1(n, 1, deg1);
  unsigned int deg2[n] = {1,2};
  Monom<double> mon2(n, -2, deg2);
  unsigned int deg3[n] = {0,2};
  Monom<double> mon3(n, 1, deg3);
  unsigned int deg4[n] = {3,1};
  Monom<double> mon4(n, -2, deg4);
  unsigned int deg5[n] = {2,1};
  Monom<double> mon5(n, 4, deg5);
  unsigned int deg6[n] = {1,1};
  Monom<double> mon6(n, -2, deg6);
  unsigned int deg7[n] = {4,0};
  Monom<double> mon7(n, 1, deg7);
  unsigned int deg8[n] = {3,0};
  Monom<double> mon8(n, -2, deg8);
  unsigned int deg9[n] = {2,0};
  Monom<double> mon9(n, 1, deg9);
  unsigned int deg10[n] = {0,2};
  Monom<double> mon10(n, 1, deg10);
  unsigned int deg11[n] = {0,1};
  Monom<double> mon11(n, 2, deg11);


  poly.addMonom(mon1);
  poly.addMonom(mon2);
  poly.addMonom(mon3);
  poly.addMonom(mon4);
  poly.addMonom(mon5);
  poly.addMonom(mon6);
  poly.addMonom(mon7);
  poly.addMonom(mon8);
  poly.addMonom(mon9);
  poly.addMonom(mon10);
  poly.addMonom(mon11);
  PolynomTxt fmt;
  std::string str = PolyUtil::toString(fmt, poly);
  printf("polynom: %s\n", str.c_str());
  double vars[n] = {1, 1};
  printf("value in 1, 1: %lf\n", poly.getValue(vars));
  PolyLipObjective<double> pobj(&poly);
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  double eps = .0000001;
  GOInitialData<double> id;
  LipzFuncDiscarder<double> dscf;
  Fact fact;
  SegVecDecomp<Fact> decomp;

  dscf.setEps(eps);
  dscf.setObjective(&pobj);

  id.mObj = &pobj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  //id.mType = GOInitialData<double>::BOXCONSTRAINED;
  for(int i = 0; i < n; i ++) {
    id.mA[i] = -10;
    id.mB[i] = 10;
  }
  fact.setInitialData(&id);
  fact.addDiscarder(&dscf);
  fact.addDiscarder(&decomp);
  Solver solver(&fact);
  solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
  solver.solve();
  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());

  LipzGradDiscarder<double> dscg;
  dscg.setObjective(&pobj);
  dscg.setEps(eps);
  dscg.setInitialData(&id);
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_GRAD;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_CONV;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_GRAD_BALL_CUT;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_UNCONS_BALL_CUT;

  //dscg.getDebugOptions() |= LipzGradDiscarder<double>::DebugOptions::PRINT_BOUNDS;
  Fact factg;
  factg.setInitialData(&id);
  factg.addDiscarder(&dscg);
  factg.addDiscarder(&decomp);
  Solver solverg(&factg);
  solverg.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
  solverg.solve();
  printf("Minimum = %s\n", solverg.getSolutionContainer()->top().toString().c_str());

  return 0;
}
