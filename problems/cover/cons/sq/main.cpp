#include <iostream>
#include <problems/cover/common/go.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/segvecdecomp.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <problems/cover/common/simplecons.hpp>
#include <kernel/bnblib/seq.hpp>
#include "f.hpp"
#include "c.hpp"
#include "defs.hpp"

#define N 5
#define EPS 0.0001
#define DELTA 0.0001

void init(int n, LipzObjective<FT>* obj, GOInitialData<FT>& id)
{
  id.mObj = obj;
  id.mType = GOInitialData<FT>::GENERCONSTRAINED;
  id.mA = (FT*) malloc(n * sizeof(FT));
  id.mB = (FT*) malloc(n * sizeof(FT));
  for(int i = 0; i < n; i ++) {
    id.mA[i] = -1;
    id.mB[i] = 1;
  }
}

C* getConstraint(int code, int n) 
{
  FT* z = new FT[n];
  for(int i = 0; i < n; i ++) {
    int mask = 1 << i;
    if(code & mask) 
      z[i] = 1.;
    else
      z[i] = -1.;
  }
  VecUtils::vecPrint(n, z);
  C* c = new C(n, z);
  return c;
}

void addConstraints(int n, SimpleConstraintChecker<FT>& scheck, GOFactory<FT>& fact, GOInitialData<FT>& id)
{
  int m = 1 << n;
  for(int i = 0; i < m; i ++) {
    C *c = getConstraint(i, n);
    scheck.addInequalityConstraint(c, DELTA);
    LipzGradDiscarder<FT> *dscgc = new LipzGradDiscarder<FT>();
    dscgc->setObjective(c);
    dscgc->setInitialData(&id);
//    dscgc->getDebugOptions() |= LipzGradDiscarder<FT>::DebugOptions::PRINT_BOUNDS;
    std::ostringstream os;
    os << "Constraint " << i;
    dscgc->setName(os.str());
    dscgc->getOptions() |= LipzGradDiscarder<FT>::Options::DO_PRIMARY_BALL_CUT;

    fact.addDiscarder(dscgc);
  }
}

main(int argc, char* argv[])
{
  typedef GOFactory < FT > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  int n = N;
  FT z[n];
  F f(n);
  GOInitialData<FT> id;
  SegVecDecomp<Fact> decomp;
  LipzGradDiscarder<FT> dscg;
  dscg.setName("Objective");
  
  
  SimpleConstraintChecker<FT> scheck;
  Fact fact;
  
  init(n, &f, id);
  fact.setInitialData(&id);
  addConstraints(n, scheck, fact, id);
  fact.setConstraintChecker(&scheck);
  
  dscg.setEps(EPS);
  dscg.setInitialData(&id);
//  dscg.getDebugOptions() |= LipzGradDiscarder<FT>::DebugOptions::PRINT_BOUNDS;
  dscg.setObjective(&f);
  dscg.getOptions() &= ~LipzGradDiscarder<FT>::Options::CHECK_GRAD;
  dscg.getOptions() &= ~LipzGradDiscarder<FT>::Options::CHECK_GRAD_COMP;
  dscg.getOptions() &= ~LipzGradDiscarder<FT>::Options::UPDATE_RECORD;
  dscg.getOptions() |= LipzGradDiscarder<FT>::Options::DO_PRIMARY_BALL_CUT;

  fact.addDiscarder(&dscg);
  fact.addDiscarder(&decomp);
  //fact.getDebugOptions() |= Fact::DebugOptions::DO_DEBUG;
  Solver solver(&fact);
  solver.setDebugOptions(
                         Solver::Options::PRINT_RESULTING_STAT 
//        | Solver::Options::PRINT_STATE
                        );
  //solver.Traverse::setDebugOptions(Traverse::Options::PRINT_STEP);
//   solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT | Solver::Options::PRINT_INITIAL_VAL);
  //solver.setRecord(n - 1);
  solver.solve();  
  if(solver.getSolutionContainer()->empty())
    printf("Empty container\n");
  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
}
