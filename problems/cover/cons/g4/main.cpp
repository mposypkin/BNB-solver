#include <problems/cover/common/go.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/segvecdecomp.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <problems/cover/common/simplecons.hpp>
#include <kernel/bnblib/seq.hpp>
#include "c1.hpp"
#include "c2.hpp"
#include "c3.hpp"
#include "c4.hpp"
#include "c5.hpp"
#include "c6.hpp"
#include "f.hpp"


void init(int n, LipzObjective<double>* obj, GOInitialData<double>& id)
{
  id.mObj = obj;
  id.mType = GOInitialData<double>::GENERCONSTRAINED;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  id.mA[0] = 78.;
  id.mA[1] = 33.;
  id.mA[2] = 27.;
  id.mA[3] = 27.;
  id.mA[4] = 27.;
  id.mB[0] = 102.;
  id.mB[1] = 45.;
  id.mB[2] = 45.;
  id.mB[3] = 45.;
  id.mB[4] = 45.;  
}

main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  int n = 5;
  F f;
  C1 c1;
  C2 c2;
  C3 c3;
  C4 c4;
  C5 c5;
  C6 c6;
  GOInitialData<double> id;
  //Decomposer<Fact> decomp;
  SegVecDecomp<Fact> decomp;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  
  LipzFuncDiscarder<double> dsccons1;
  LipzFuncDiscarder<double> dsccons2;
  LipzFuncDiscarder<double> dsccons3;
  LipzFuncDiscarder<double> dsccons4;
  LipzFuncDiscarder<double> dsccons5;
  LipzFuncDiscarder<double> dsccons6;
  
  LipzGradDiscarder<double> dscgcons1;
  LipzGradDiscarder<double> dscgcons2;
  LipzGradDiscarder<double> dscgcons3;
  LipzGradDiscarder<double> dscgcons4;
  LipzGradDiscarder<double> dscgcons5;
  LipzGradDiscarder<double> dscgcons6;
  
  SimpleConstraintChecker<double> scheck;
  Fact fact;
  double eps = 0.000001;
  double delta = 0.000001;
  
  init(n, &f, id);
  fact.setInitialData(&id);
  fact.addDiscarder(&decomp);

  scheck.addInequalityConstraint(&c1, delta);
  scheck.addInequalityConstraint(&c2, delta);
  scheck.addInequalityConstraint(&c3, delta);
  scheck.addInequalityConstraint(&c4, delta);
  scheck.addInequalityConstraint(&c5, delta);
  scheck.addInequalityConstraint(&c6, delta);
  fact.setConstraintChecker(&scheck);
  
  dscf.setEps(eps);
  dscf.setObjective(&f);
  
  dscg.setEps(eps);
  dscg.setObjective(&f);
  dscg.setInitialData(&id);
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::UPDATE_RECORD;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;

  
  /*
  dsccons1.setObjective(&c1);
  dsccons2.setObjective(&c2);
  dsccons3.setObjective(&c3);
  dsccons4.setObjective(&c4);
  dsccons5.setObjective(&c5);
  dsccons6.setObjective(&c6);
  
  
  fact.addDiscarder(&dsccons1);
  fact.addDiscarder(&dsccons2);
  fact.addDiscarder(&dsccons3);
  fact.addDiscarder(&dsccons4);
  fact.addDiscarder(&dsccons5);
  fact.addDiscarder(&dsccons6);
 
  fact.addDiscarder(&dscf);
  */
  
  
  
  dscgcons1.setObjective(&c1);
  dscgcons2.setObjective(&c2);
  dscgcons3.setObjective(&c3);
  dscgcons4.setObjective(&c4);
  dscgcons5.setObjective(&c5);
  dscgcons6.setObjective(&c6);

  dscgcons1.setInitialData(&id);
  dscgcons2.setInitialData(&id);
  dscgcons3.setInitialData(&id);
  dscgcons4.setInitialData(&id);
  dscgcons5.setInitialData(&id);
  dscgcons6.setInitialData(&id);

  dscgcons1.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
  dscgcons2.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
  dscgcons3.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
  dscgcons4.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
  dscgcons5.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
  dscgcons6.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;

  fact.addDiscarder(&dscgcons1);
  fact.addDiscarder(&dscgcons2);
  fact.addDiscarder(&dscgcons3);
  fact.addDiscarder(&dscgcons4);
  fact.addDiscarder(&dscgcons5);
  fact.addDiscarder(&dscgcons6);
  fact.addDiscarder(&dscg);
  
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
  scheck.print((double*)solver.getSolutionContainer()->top().mX);
  double ref[5] = {78., 33., 29.995, 45., 36.7758};
  scheck.print(ref);
  printf("Ref = %lf\n", f.func(ref));

}
