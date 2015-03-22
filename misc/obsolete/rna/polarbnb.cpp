#include <stdio.h>
#include <problems/go/go.hpp>
#include "rnapolar.hpp"
#include <kernel/bnblib/wtraverse.hpp>
#include <problems/go/lipzfdiscarder.hpp>
#include <problems/go/lipzgdiscarder.hpp>
#include <kernel/bnblib/seq.hpp>
#include "rnagen.hpp"
#include "sultan.hpp"
#include <problems/optlib/fletchsearch.hpp>
#include <problems/optlib/steepdesc.hpp>
#include <problems/optlib/conjgrad.hpp>

void init(int n, LipzObjective<double>& obj, GOInitialData<double>& id)
{
  id.mObj = &obj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  for(int i = 0; i < n; i ++) {
    id.mA[i] = 0.;
    id.mB[i] = M_PI;
  }
/*  id.mB[0] = 0.5 * M_PI;
  id.mB[n - 1]  = 2 * M_PI;*/
}

main(int argc, char* argv[]) {  
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  ConjGrad<double> sd;
  FletchSearch<double> ls;
  int n;
  double eps, epsloc = 0.001;
  RNABase<double> rnabase;
  RNAPolar obj;
  GOInitialData<double> id;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  Fact fact;
  BNB_ASSERT(argc >= 2);
  eps = atof(argv[1]);
  //initSultan2(n, rnabase);
  RnaGenerator::generate("gggggggggg", n, rnabase);
  obj.setRNABase(&rnabase);
  dscf.setEps(eps);
  dscf.setObjective(&obj);
  dscg.setEps(eps);
  dscg.setObjective(&obj);
  dscg.setInitialData(&id);
  init(n - 1, obj, id);
  fact.setInitialData(&id);
//   fact.addDiscarder(&dscf);
  fact.addDiscarder(&dscg);
  ls.setEps(0.5 * epsloc);
  ls.setRightEnd(32.);
  ls.setRho(0.05);
  ls.setSigma(0.3);
  sd.setLineSearch(&ls);
  sd.setObjective(&obj);
  sd.setMaxIters(10000);
  sd.setEps(epsloc);
  fact.setLocalOptimizer(&sd);
  fact.getOptions() |= Fact::Options::DO_LOCAL_SEARCH;
  Solver solver(&fact);
  solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
  solver.solve();  
  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
}
