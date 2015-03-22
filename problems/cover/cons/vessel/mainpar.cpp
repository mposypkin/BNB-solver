#include <problems/cover/common/go.hpp>
#include <problems/cover/common/gopack.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <problems/cover/common/simplecons.hpp>
#include <kernel/bnblib/wtraverse.hpp>
#include <kernel/bnblib/simplshed.hpp>
// #include <kernel/bnblib/msheur.hpp>
#include <kernel/bnblib/msheur2.hpp>
#include <kernel/bnblib/basicheurmaster.hpp>
#include <kernel/bnblib/simplsuf.hpp>
#include <util/aclib/mpiaclib.hpp>
#include "c1.hpp"
#include "c2.hpp"
#include "c3.hpp"
#include "f.hpp"


void init(int n, LipzObjective<double>* obj, GOInitialData<double>& id)
{
  id.mObj = obj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  id.mA[0] = 0.;
  id.mA[1] = 0.;
  id.mA[2] = 0.;
  id.mA[3] = 0.;
  
  id.mB[0] = 100.;
  id.mB[1] = 100.;
  id.mB[2] = 200.;
  id.mB[3] = 240.;
}

main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBMsHeurSolver < Traverse > Solver;
  int n = 4;
  F f;
  C1 c1;
  C2 c2;
  C3 c3;
  GOInitialData<double> id;
  Decomposer<Fact> decomp;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  
  LipzFuncDiscarder<double> dsccons1;
  LipzFuncDiscarder<double> dsccons2;
  LipzFuncDiscarder<double> dsccons3;
  
  LipzGradDiscarder<double> dscgcons1;
  LipzGradDiscarder<double> dscgcons2;
  LipzGradDiscarder<double> dscgcons3;
  
  SimpleConstraintChecker<double> scheck;
  Fact fact;
  double eps = 0.1;
  double delta = 0.1;
  AbstractCommunicator ac(&argc, &argv);
  
  init(n, &f, id);
  fact.setInitialData(&id);
  fact.addDiscarder(&decomp);

  scheck.addInequalityConstraint(&c1, delta);
  scheck.addInequalityConstraint(&c2, delta);
  scheck.addInequalityConstraint(&c3, delta);
  fact.setConstraintChecker(&scheck);
  
  dscf.setEps(eps);
  dscf.setObjective(&f);
  
  dscg.setEps(eps);
  dscg.setObjective(&f);
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::UPDATE_RECORD;
  
  dsccons1.setObjective(&c1);
  dsccons1.getOptions() |= Objective<double>::INEQCONSTRAINT;
  
  dsccons2.setObjective(&c2);
  dsccons2.getOptions() |= Objective<double>::INEQCONSTRAINT;
  
  dsccons3.setObjective(&c3);
  dsccons3.getOptions() |= Objective<double>::INEQCONSTRAINT;
  
  
 /* fact.addDiscarder(&dsccons1);
  fact.addDiscarder(&dsccons2);
  fact.addDiscarder(&dsccons3);
 
  fact.addDiscarder(&dscf);*/
  
  
  
  dscgcons1.setObjective(&c1);
  dscgcons2.setObjective(&c2);
  dscgcons3.setObjective(&c3);

  fact.addDiscarder(&dscgcons1);
  fact.addDiscarder(&dscgcons2);
  fact.addDiscarder(&dscgcons3);
  fact.addDiscarder(&dscg);
  Solver solver(&ac, &fact, ac.getProcessNumber() - 3);  
  SimpleScheduler sched;     
  sched.setMasterTreshold(0);
  sched.setSlaveTreshold(1000);
  sched.setSlaveChunk(100);
  sched.setMinQueueLength(1);
  sched.setMaxQueueLength(1000);
  solver.setScheduler(&sched);
  BNBBasicHeurMaster < Fact > hm;
  //hm.setUpdateMode(BNBBasicHeurMaster < Fact >::UpdateModes::NO_UPDATE);
  //hm.setUpdateMode(BNBBasicHeurMaster < Fact >::UpdateModes::DO_UPDATE_FROM);
  solver.setHeurMaster(&hm);
  SimpleSolutionUpdateFilter <Fact> uf;
  solver.setSolutionUpdateFilter(&uf);

  solver.solve();  
  if(ac.getMyId() == 0) {
    if(solver.getSolutionContainer()->empty())
      printf("Empty container\n");
    printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
    scheck.print((double*)solver.getSolutionContainer()->top().mX);
    double Sandren[4] = {1.125, 0.625, 47.70, 117.701};
    scheck.print(Sandren);
    printf("Sandren = %lf\n", f.func(Sandren));
    double Coello[4] = {0.8125, 0.4375, 42.097398, 176.654047};
    scheck.print(Coello);
    printf("Coello = %lf\n", f.func(Coello));
  }
  
}
