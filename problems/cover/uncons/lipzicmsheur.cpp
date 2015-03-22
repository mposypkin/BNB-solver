#include <problems/cover/common/go.hpp>
#include <problems/cover/common/gopack.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include "rosenbrokn.hpp"
#include <kernel/bnblib/wtraverse.hpp>
#include <kernel/bnblib/msheur2.hpp>
#include <kernel/bnblib/simplshed.hpp>
#include <kernel/bnblib/basicheurmaster.hpp>
#include <kernel/bnblib/simplsuf.hpp>
#include <problems/optlib/fletchsearch.hpp>
#include <problems/optlib/conjgrad.hpp>
#include <problems/cover/common/contheur.hpp>
#include <util/aclib/mpiaclib.hpp>


void init(int n, LipzObjective<double>& obj, GOInitialData<double>& id)
{
  id.mObj = &obj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  for(int i = 0; i < n; i ++) {
    id.mA[i] = -n;
    id.mB[i] = n;
  }
}

main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBMsHeurSolver < Traverse > Solver;
  
  int n;
  double eps, eps1;
  Rosenbrokn obj(2);
  GOInitialData<double> id;
  Decomposer<Fact> decomp;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  
  AbstractCommunicator ac(&argc, &argv);
  Fact fact;
  
  BNB_ASSERT(argc >= 3);
  n = atoi(argv[1]);
  eps = atof(argv[2]);
  eps1 = 0.0001;
  obj.setDim(n);
  dscf.setEps(eps);
  dscf.setObjective(&obj);
  dscg.setInitialData(&id);
  dscg.setEps(eps);
  dscg.setObjective(&obj);
  init(n, obj, id);
  fact.setInitialData(&id);
  fact.addDiscarder(&decomp);
  //fact.addDiscarder(&dscf);
  fact.addDiscarder(&dscg);
  Solver solver(&ac, &fact, ac.getProcessNumber() - 3);  
  SimpleScheduler sched;     
  sched.setMasterTreshold(0);
  sched.setSlaveTreshold(100);
  sched.setSlaveChunk(10);
  sched.setMinQueueLength(1);
  sched.setMaxQueueLength(1000);
  solver.setScheduler(&sched);
  SimpleSolutionUpdateFilter <Fact> uf;
  solver.setSolutionUpdateFilter(&uf);
  solver.setCheckRatio(1);
  BNBBasicHeurMaster < Fact > hm;
  //hm.setUpdateMode(BNBBasicHeurMaster < Fact >::UpdateModes::NO_UPDATE);
  //hm.setUpdateMode(BNBBasicHeurMaster < Fact >::UpdateModes::DO_UPDATE_FROM);
  solver.setHeurMaster(&hm);

  ConjGrad<double> sd;
  FletchSearch<double> ls;    
  sd.setLineSearch(&ls);
  ls.setEps(eps1 * 0.5);
  ls.setRightEnd(.1);
  ls.setRho(0.2);
  ls.setSigma(0.4);
  sd.setMaxIters(10000);
  sd.setObjective(id.mObj);
  sd.setEps(eps1);
  ContHeur < Fact > hs;
  hs.setInitialData(&id);
  hs.setAmplitude(0);
  hs.setLocalOptimizer(&sd);
  hs.setNumberOfStarts(1);  
  solver.setHeurSlave(&hs);

  
//   solver.setOptions(Solver::Options::PRINT_RESULTING_STAT);
  solver.solve();  
  if(ac.getMyId() == 0) {
    printf("Incumbent value = %lf\n", solver.getRecord());
    printf("Incumbent solution = %s\n", solver.getSolutionContainer()->top().toString().c_str());
  }
}


