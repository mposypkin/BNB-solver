#include <common/go.hpp>
#include <common/decomposer.hpp>
#include <common/segvecdecomp.hpp>
#include <common/lipzfdiscarder.hpp>
#include <common/lipzgdiscarder.hpp>
#include <pcommon/simplecons.hpp>
#include <kernel/bnblib/seq.hpp>
#include "c1.hpp"
#include "c2.hpp"
#include "f.hpp"
#include <common/intsolumak.hpp>
#include <common/intstepdisc.hpp>

#define MIXED_INTEGER 0
#define INNER_POINT 0
#define LIP_FUNC 0
#define LIP_GRAD 1
#define EPS 0.1
#define DELT 0.1
#define DO_BALL_CUT 1
#define SET_DISCRETE_STEP 0
#define DISCRETE_STEP .01

typedef GOFactory < double > Fact;
typedef WTraverse < Fact > Traverse;
typedef BNBSeqSolver < Traverse > Solver;

class TuySolutionContainer : public SolutionContainer <Fact>
{
  public:
    TuySolutionContainer()
    {
      mHasSolution = false;
    }

    void push(const Solution & s)
    {
      if(s.getValue() < mDeltaFeasible.getValue()){
	mDeltaFeasible = s;
	mHasSolution = true;
      }

      if((mC1.func(s.mX) <= 0) && (mC2.func(s.mX) <= 0))
      {
	printf("s = %s\n", s.toString().c_str());
	if(s.getValue() < mFeasible.getValue()){
	  mFeasible = s;
	}
      }

    }

    bool empty() const
    {
      return !mHasSolution;
    }

    const Solution& top() const
    {
      return mDeltaFeasible;
    }

    Solution getFeasible() const
    {
      return mFeasible;
    }

  private:

    bool mHasSolution;
    Solution mDeltaFeasible;
    Solution mFeasible;
    F mF;
    C1 mC1;
    C2 mC2;
};

void init(int n, LipzObjective<double>* obj, GOInitialData<double>& id)
{
  id.mObj = obj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  id.mA[0] = -10.;
  id.mB[0] = 10.;
  id.mA[1] = -10.;
  id.mB[1] = 10.;
  id.mA[2] = -10.;
  id.mB[2] = 10.;
  id.mType = GOInitialData<double>::GENERCONSTRAINED;
}

main(int argc, char* argv[])
{
  int n = 3;
  F f;
  C1 c1;
  C2 c2;
  GOInitialData<double> id;
  //Decomposer<Fact> decomp;
  SegVecDecomp<Fact> decomp;
  LipzFuncDiscarder<double> dscf;
  LipzFuncDiscarder<double> dscfcons1;
  LipzFuncDiscarder<double> dscfcons2;
  LipzGradDiscarder<double> dscg;
  LipzGradDiscarder<double> dscgcons1;
  LipzGradDiscarder<double> dscgcons2;
  SimpleConstraintChecker<double> scheck;
  IntStepDiscarder<double> stepd(3);
  IntSolumak<double> solumak(3);
  TuySolutionContainer tsc;
  Fact fact;
  double eps = EPS;
  double delta = DELT;
  
  
#if SET_DISCRETE_STEP
  std::map<int, double> steps;
  for(int i = 0; i < 3; i ++) {
    steps[i] = DISCRETE_STEP;
  }
  solumak.setSteps(steps);
  stepd.setSteps(steps); 
#endif

#if INNER_POINT 
  c1.setDelt(delta);
  c2.setDelt(delta);
#else
  c1.setDelt(0);
  c2.setDelt(0);
#endif

  scheck.addInequalityConstraint(&c1, delta);
  scheck.addInequalityConstraint(&c2, delta);
  
  dscf.setName("Optimality Lipschitz Discarder\n");
  dscf.setEps(eps);
  dscf.setObjective(&f);
  dscf.getOptions() &= ~LipzFuncDiscarder<double>::Options::UPDATE_RECORD;

  dscfcons1.setName("Contstraint 1 Lipschitz discarder\n");
  dscfcons1.setObjective(&c1);
  dscfcons1.getOptions() &= ~LipzFuncDiscarder<double>::Options::UPDATE_RECORD;

  dscfcons2.setName("Constraint 2 Lipschitz discarder\n");
  dscfcons2.setObjective(&c2);
  dscfcons2.getOptions() &= ~LipzFuncDiscarder<double>::Options::UPDATE_RECORD;

  dscg.setName("Optimality Lipschitz gradient discarder");
  dscg.setEps(eps);
  dscg.setObjective(&f);
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::UPDATE_RECORD;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::EVAL_UNCONS_BOUND;
#if DO_BALL_CUT
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
#endif
  dscg.setInitialData(&id);

  
  dscgcons1.setName("Constraint 1 Lipschitz gradient discarder");
  dscgcons1.setObjective(&c1);
  dscgcons1.setInitialData(&id);
  dscgcons1.getOptions() &= ~LipzGradDiscarder<double>::Options::UPDATE_RECORD;
  dscgcons1.getOptions() &= ~LipzGradDiscarder<double>::Options::EVAL_UNCONS_BOUND;
#if DO_BALL_CUT
  dscgcons1.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
#endif

  dscgcons2.setName("Constraint 2 Lipschitz gradient discarder");
  dscgcons2.setObjective(&c2);
  dscgcons2.setInitialData(&id);
  dscgcons2.getOptions() &= ~LipzGradDiscarder<double>::Options::UPDATE_RECORD;
  dscgcons2.getOptions() &= ~LipzGradDiscarder<double>::Options::EVAL_UNCONS_BOUND;
#if DO_BALL_CUT
  dscgcons2.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
#endif
  
  init(n, &f, id);
  fact.setInitialData(&id);
  fact.setConstraintChecker(&scheck);
  fact.addDiscarder(&decomp);
#if MIXED_INTEGER
  fact.setSolutionMaker(&solumak);
  fact.addDiscarder(&stepd);
#endif

#if LIP_FUNC
  fact.addDiscarder(&dscfcons1);
  fact.addDiscarder(&dscfcons2);
  fact.addDiscarder(&dscf);
#endif

#if LIP_GRAD
  fact.addDiscarder(&dscgcons1);
  fact.addDiscarder(&dscgcons2);
  fact.addDiscarder(&dscg);
#endif

  Solver solver(&fact);
  solver.setSolutionContainer(&tsc);
  solver.setDebugOptions(
      Solver::Options::PRINT_RESULTING_STAT 
//        | Solver::Options::PRINT_STATE
      );
  //solver.Traverse::setDebugOptions(Traverse::Options::PRINT_STEP);
  //fact.getDebugOptions() |= Fact::DebugOptions::DO_DEBUG;
//   solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT | Solver::Options::PRINT_INITIAL_VAL);
  //solver.setRecord(n - 1);
  solver.solve();  
  if(solver.getSolutionContainer()->empty())
    printf("Empty container\n");
  else {
    printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
    scheck.print((double*)solver.getSolutionContainer()->top().mX);
    printf("Feasible Minimum = %s\n", tsc.getFeasible().toString().c_str());
    scheck.print((double*)tsc.getFeasible().mX);
  }

  printf("Tuy: \n");
  double tuy[3] = {3.747692, 7.171420, 2.362317};
  printf("Minimum = %lf\n", f.func(tuy));
  scheck.print(tuy);

}
