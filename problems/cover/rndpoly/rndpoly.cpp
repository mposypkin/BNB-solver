#include <stdio.h>
#include <stdlib.h>
#include <problems/cover/poly/polylipobj.hpp>
#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polynomgams.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/optlib/conjgrad.hpp>
#include <problems/optlib/fletchsearch.hpp>
#include <problems/cover/common/go.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/segvecdecomp.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <kernel/bnblib/seq.hpp>


#define M 3
#define A 10
#define D 8

typedef double DOUBLE;

int B;

void solvePoly(Polynom<DOUBLE>& poly)
{
  PolynomGams fmt;  
  std::string str = PolyUtil::toString(fmt, poly);

  printf("polynom: %s\n", str.c_str());
  PolyLipObjective<DOUBLE> pobj(&poly);
  typedef GOFactory < DOUBLE > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  DOUBLE eps = .0001;
  GOInitialData<DOUBLE> id;
  LipzFuncDiscarder<DOUBLE> dscf;
  Fact fact;
  SegVecDecomp<Fact> decomp;

  dscf.setEps(eps);
  dscf.setObjective(&pobj);

  id.mObj = &pobj;
  id.mA = (DOUBLE*) malloc(M * sizeof(DOUBLE));
  id.mB = (DOUBLE*) malloc(M * sizeof(DOUBLE));
  //id.mType = GOInitialData<DOUBLE>::BOXCONSTRAINED;
  printf("B = %d\n", B);
  for(int i = 0; i < M; i ++) {
    id.mA[i] = -B;
    id.mB[i] = B;
  }
  fact.setInitialData(&id);
  fact.addDiscarder(&dscf);
  fact.addDiscarder(&decomp);
  Solver solver(&fact);
  solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
  
#if 0
  solver.solve();
  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
#endif

  LipzGradDiscarder<DOUBLE> dscg;
  dscg.setObjective(&pobj);
  dscg.setEps(eps);
  dscg.setInitialData(&id);
  //dscg.getOptions() &= ~LipzGradDiscarder<DOUBLE>::Options::USE_EIGEN_VALUES;
  dscg.getOptions() &= ~LipzGradDiscarder<DOUBLE>::Options::EVAL_UNCONS_BOUND;
  //dscg.getOptions() |= LipzGradDiscarder<DOUBLE>::Options::CHECK_GRAD;

  //dscg.getOptions() |= LipzGradDiscarder<DOUBLE>::Options::CHECK_GRAD_COMP;
  //dscg.getOptions() |= LipzGradDiscarder<DOUBLE>::Options::CHECK_CONV;
  dscg.getOptions() |= LipzGradDiscarder<DOUBLE>::Options::DO_GRAD_BALL_CUT;
  dscg.getOptions() |= LipzGradDiscarder<DOUBLE>::Options::DO_PRIMARY_BALL_CUT;
  dscg.getOptions() |= LipzGradDiscarder<DOUBLE>::Options::DO_UNCONS_BALL_CUT;

  //dscg.getDebugOptions() |= LipzGradDiscarder<DOUBLE>::DebugOptions::PRINT_BOUNDS;
  Fact factg;
  factg.setInitialData(&id);
  factg.addDiscarder(&dscg);
  factg.addDiscarder(&decomp);
  Solver solverg(&factg);
  solverg.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
#if 1  
  solverg.solve();
  printf("Minimum = %s\n", solverg.getSolutionContainer()->top().toString().c_str());
#endif

}

void makePoly(int d, int k, Polynom<DOUBLE> &poly)
{
  unsigned int deg[M];
  B = 0;
  for(int i = 0; i < M; i ++) {
    DOUBLE coe;
    for(int j = 0; j < M; j ++) 
      deg[j] = 0;
    deg[i] = d;
    Monom<DOUBLE> mon(M, A, deg);
    poly.addMonom(mon);
  }
  PolyLipObjective<DOUBLE> pobj(&poly);
  for(int j = 0; j < M; j ++) 
    deg[j] = 0;
  while(1) {
    bool carry = false;
    for(int i = 0; i < M; i ++) {
      if(deg[i] < k) {
	deg[i] ++;
	carry = false;
	break;
      } else {
	deg[i] = 0;
	carry = true;
      }
    }
    unsigned int s = 0;
    for(int i = 0; i < M; i ++) 
      s += deg[i];
    if(s <= k) {
      DOUBLE coe = (1. - ((DOUBLE)rand() / (DOUBLE)RAND_MAX)) * A;
      Monom<DOUBLE> mon(M, coe, deg);
      poly.addMonom(mon);
      B ++;
    }
    if(carry)
      break;
  }
}
main()
{
  srand(1);
  for(int i = 0; i < 10; i ++) {
    Polynom<DOUBLE> poly;
    makePoly(D, D - 1, poly);
    solvePoly(poly);
  }
}
