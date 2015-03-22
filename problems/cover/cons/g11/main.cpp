#include <problems/cover/common/go.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <problems/cover/common/simplecons.hpp>
#include <kernel/bnblib/seq.hpp>
#include "c1.hpp"
#include "f.hpp"


void init(int n, LipzObjective<double>* obj, GOInitialData<double>& id)
{
  id.mObj = obj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  id.mType = GOInitialData<double>::GENERCONSTRAINED;
  for(int i = 0; i < n; i ++) {
    id.mA[i] = -1;
    id.mB[i] = 1;
  }
}

main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  int n = 2;
  F f;
  C1 c1;
  GOInitialData<double> id;
  Decomposer<Fact> decomp;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  LipzFuncDiscarder<double> dsccons;
  LipzGradDiscarder<double> dscgcons;
  SimpleConstraintChecker<double> scheck;
  Fact fact;
  double eps = 0.00001;
  double delta = 0.00001;
  
  scheck.addEqualityConstraint(&c1, delta);
  
  dscf.setEps(eps);
  dscf.setObjective(&f);
  
  dscg.setEps(eps);
  dscg.setObjective(&f);
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::UPDATE_RECORD;
  
  dsccons.setObjective(&c1);
  dscgcons.setObjective(&c1);
  
  init(n, &f, id);
  fact.setInitialData(&id);
  fact.setConstraintChecker(&scheck);
  fact.addDiscarder(&decomp);
  fact.addDiscarder(&dsccons);
//   fact.addDiscarder(&dscgcons);
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
  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
  
}
