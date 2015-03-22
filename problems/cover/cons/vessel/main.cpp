#include <util/common/simpmemmanager.hpp>
#include <kernel/bnblib/wtraverse.hpp>
#include <kernel/bnblib/ptraverse.hpp>
#include <problems/cover/common/go.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/segvecdecomp.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <problems/cover/common/linconsdiscarder.hpp>
#include <problems/cover/common/simplecons.hpp>
#include <kernel/bnblib/seq.hpp>
#include "c1.hpp"
#include "c2.hpp"
#include "c3.hpp"
#include "f.hpp"
#include <problems/cover/common/intsolumak.hpp>
#include <problems/cover/common/intstepdisc.hpp>


#define MAX_MEM_SIZE 64 * 1024 * 1024

//#define PARALLEL_VERSION

void init(int n, LipzObjective<double>* obj, GOInitialData<double>& id)
{
  id.mObj = obj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  id.mA[0] = 0.;
  id.mA[1] = 0.;
  id.mA[2] = 0.;
  id.mA[3] = 0.;
  
  id.mB[0] = 200.;
  id.mB[1] = 200.;
  id.mB[2] = 200.;
  id.mB[3] = 240.;
  id.mType = GOInitialData<double>::GENERCONSTRAINED;
  //id.mB[3] = 200.;
}

void correct(double* v)
{
  v[0] = ((int)(v[0] / 0.0625) + 1) * 0.0625;
  v[1] = ((int)(v[1] / 0.0625) + 1) * 0.0625;
}

main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
#ifdef PARALLEL_VERSION  
  typedef PTraverse < Fact > Traverse;
#else  
  typedef WTraverse < Fact > Traverse;
#endif  
//   typedef WTraverse < Fact, std::stack <Fact::Set> >  Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  int n = 4;
  double eps = 0.0000001;
  double delta = 0.000000;
  F f;
  C1 c1;
  C2 c2;
  C3 c3;
  LinObjective<double> lo1;
  lo1.setDim(4);
  double lcoe1[5] = {-1., 0., 0.0193, 0., 0.};
  lo1.setCoeff(lcoe1);
  LinObjective<double> lo2;
  lo2.setDim(4);
  double lcoe2[5] = {0., -1., 0.00954, 0., 0.};
  lo2.setCoeff(lcoe2);
  
  
  //c1.setTolerance(2  * delta);

  GOInitialData<double> id;
  SegVecDecomp<Fact> decomp;
  //Decomposer<Fact> decomp;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  
  LipzFuncDiscarder<double> dsccons1;
  LipzFuncDiscarder<double> dsccons2;
  LipzFuncDiscarder<double> dsccons3;
  
  LipzGradDiscarder<double> dscgcons1;
  LipzGradDiscarder<double> dscgcons2;
  LipzGradDiscarder<double> dscgcons3;

  LinConsDiscarder<double> lcons1;
  LinConsDiscarder<double> lcons2;
  
  SimpleConstraintChecker<double> scheck;

  Fact fact;

  std::map<int, double> steps;
  steps[0] = 0.0625;
  steps[1] = 0.0625;
  IntStepDiscarder<double> stepd(steps);
  IntSolumak<double> solumak(steps);
  
  init(n, &f, id);
  fact.setInitialData(&id);
  fact.setSolutionMaker(&solumak);

  scheck.addInequalityConstraint(&c1, delta);
  scheck.addInequalityConstraint(&c2, delta);
  scheck.addInequalityConstraint(&c3, delta);
  fact.setConstraintChecker(&scheck);
  
  dscf.setEps(eps);
  dscf.setObjective(&f);
  
  //decomp.setBallExcludeLevel(0);
  dscg.setEps(eps);
  dscg.setObjective(&f);
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::UPDATE_RECORD;
  //dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;

  dscg.setInitialData(&id);
  
  dsccons1.setObjective(&c1);
  dsccons2.setObjective(&c2);
  dsccons3.setObjective(&c3);
  
  
 /* fact.addDiscarder(&dsccons1);
  fact.addDiscarder(&dsccons2);
  fact.addDiscarder(&dsccons3);
 
  fact.addDiscarder(&dscf);*/
  
  
  
  dscgcons1.setObjective(&c1);
  dscgcons2.setObjective(&c2);
  dscgcons3.setObjective(&c3);
  dscgcons1.setInitialData(&id);
  dscgcons2.setInitialData(&id);
  dscgcons3.setInitialData(&id);
  //dscgcons3.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
  lcons1.setObjective(&lo1);
  lcons1.getOptions() |= LinConsDiscarder<double>::Options::DO_CUT;
  lcons2.setObjective(&lo2);
  lcons2.getOptions() |= LinConsDiscarder<double>::Options::DO_CUT;

  fact.addDiscarder(&decomp);
  fact.addDiscarder(&stepd);
#if 0
  fact.addDiscarder(&dscgcons1);
  fact.addDiscarder(&dscgcons2);
#else
  fact.addDiscarder(&lcons1);
  fact.addDiscarder(&lcons2);
#endif
  fact.addDiscarder(&dscgcons3);
  fact.addDiscarder(&dscg);
  
  
  Solver solver(&fact);

#ifdef PARALLEL_VERSION  
  if(argc < 2)
    BNB_ERROR_REPORT("Usage: main.exe #threads\n");
  int nt = atoi(argv[1]);
  solver.setNumThreads(nt);
  solver.init(MAX_MEM_SIZE, 1024 * 1024, 32);
  solver.setPutChunk(32);
  solver.setLocalSteps(100 * 1024*1024);
  solver.setUpdateRatio(128);
#endif
    
  solver.setDebugOptions(
                         Solver::Options::PRINT_RESULTING_STAT 
    //    | Solver::Options::PRINT_STATE
                        );
//  solver.Traverse::setDebugOptions(Traverse::Options::PRINT_STEP);
//   solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT | Solver::Options::PRINT_INITIAL_VAL);
  //solver.setRecord(n - 1);
  solver.solve();  
  c1.setTolerance(0.0);
  if(solver.getSolutionContainer()->empty())
    printf("Empty container\n");
  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
  scheck.print((double*)solver.getSolutionContainer()->top().mX);
  double corr[4];
  for(int i = 0; i < 4; i ++)
    corr[i] = solver.getSolutionContainer()->top().mX[i];
  correct(corr);
  printf("Corrected = [%lf, %lf, %lf, %lf]->%lf\n", corr[0], corr[1], corr[2], corr[3], f.func(corr));
  scheck.print(corr);
  double Sandren[4] = {1.125, 0.625, 47.70, 117.701};
  printf("Sandren = %lf\n", f.func(Sandren));
  scheck.print(Sandren);
  double Coello[4] = {0.8125, 0.4375, 42.097398, 176.654047};
  printf("Coello = %lf\n", f.func(Coello));
  scheck.print(Coello);
  double LINGO[4] = {0.75, 0.375, 38.860, 221.365};
  printf("LINGO = %lf\n", f.func(LINGO));
  scheck.print(LINGO);
  /*
  double Gergel[4] = {0.29, 0.32, 14.13, 145.85};
  Gergel[0] = 0.299364613880741;
  Gergel[1] = 0.323802541544474;
  Gergel[2] = 14.137341153470189;
  Gergel[3] = 145.850439882697940;
  scheck.print(Gergel);
  printf("Gergel = %lf\n", f.func(Gergel));
  */
  
}
