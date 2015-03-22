#include <problems/cover/common/go.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <problems/cover/common/simplecons.hpp>
#include <kernel/bnblib/seq.hpp>
#include "c1.hpp"
#include "c2.hpp"
#include "f.hpp"


void init(int n, LipzObjective<double>* obj, GOInitialData<double>& id)
{
  id.mObj = obj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  id.mA[0] = 13.;
  id.mB[0] = 100.;
  id.mA[1] = 0.;
  id.mB[1] = 100.;
  
}

main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  int n = 2;
  F f;
  C1 c1;
  C2 c2;
  GOInitialData<double> id;
  Decomposer<Fact> decomp;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  LipzFuncDiscarder<double> dsccons1;
  LipzGradDiscarder<double> dscgcons1;
  LipzFuncDiscarder<double> dsccons2;
  LipzGradDiscarder<double> dscgcons2;
  SimpleConstraintChecker<double> scheck;
  Fact fact;
  double eps = 0.0000000001;
  double delta = 0.0000000001;
  
  scheck.addInequalityConstraint(&c1, delta);
  scheck.addInequalityConstraint(&c2, delta);
  
  dscf.setEps(eps);
  dscf.setObjective(&f);
  
  dscg.setEps(eps);
  dscg.setObjective(&f);
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::UPDATE_RECORD;
  
  dsccons1.setObjective(&c1);
  dscgcons1.setObjective(&c1);
  dsccons2.setObjective(&c2);
  dscgcons2.setObjective(&c2);
  
  init(n, &f, id);
  fact.setInitialData(&id);
  fact.setConstraintChecker(&scheck);
  fact.addDiscarder(&decomp);
  fact.addDiscarder(&dsccons1);
//   fact.addDiscarder(&dscgcons1);
  fact.addDiscarder(&dsccons2);
//   fact.addDiscarder(&dscgcons2);
  fact.addDiscarder(&dscf);
//   fact.addDiscarder(&dscg);
  Solver solver(&fact);
  solver.setDebugOptions(
                         Solver::Options::PRINT_RESULTING_STAT 
//        | Solver::Options::PRINT_STATE
                        );
//  solver.Traverse::setDebugOptions(Traverse::Options::PRINT_STEP);
//   solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT | Solver::Options::PRINT_INITIAL_VAL);
  //solver.setRecord(n - 1);
  solver.solve();  
  if(solver.getSolutionContainer()->empty())
    printf("Empty container\n");
  
}
