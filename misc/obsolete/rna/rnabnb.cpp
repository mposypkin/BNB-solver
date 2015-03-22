#include "rna.hpp"
#include "consdiscarder.hpp"
#include <problems/go/lipzfdiscarder.hpp>
#include <problems/go/lipzgdiscarder.hpp>
#include <kernel/bnblib/wtraverse.hpp>
#include <kernel/bnblib/seq.hpp>
#include <problems/optlib/fletchsearch.hpp>
#include <problems/optlib/steepdesc.hpp>
#include "rnagen.hpp"
#include "sultan.hpp"
#include <problems/go/decomposer.hpp>

void initID(int n, RNA & rna, GOInitialData<double> & id)
{
  id.mObj = &rna;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  /*
   for(int i = 0; i < n; i ++) {
     id.mA[i] = -1.;
     id.mB[i] = 1.;
   }
   id.mA[0] = 0.;
  */
  for(int i = 0; i < n; i ++) {
    if(i % 2) {
      id.mA[i] = 0.;
      id.mB[i] = 1.;
    } else {
      id.mA[i] = -1.;
      id.mB[i] = 0.;
    }
  }
}

main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  Decomposer<Fact> decomp;
  ConsDiscarder cdsc;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  
  GOInitialData<double> id;
  Fact fact;
  Solver solver(&fact);
  RNABase<double> rnabase;
  RNA rna;
  SteepDesc<double> sd;
  FletchSearch<double> ls;
  int n;
  double eps, epsloc = 0.001;
  
  eps = atof(argv[1]);
  initSultan(n, rnabase);
  double X[4] = {-0.984385, 0.173622, -0.028999};
  printf("RNA BEST VALUE = %lf\n", rnabase.func(X));
  //initSultan2(n, rnabase);
  //RnaGenerator::generate("actg", n, rnabase);
  printf("n = %d\n", n);
  //RnaGenerator::generate("actgactg", n, rna);
  //RnaGenerator::generate("ggggg", n, rnabase);
  rna.setRNABase(&rnabase);
  initID(n, rna, id);
  fact.setInitialData(&id);
  ls.setEps(0.5 * epsloc);
  ls.setRightEnd(32.);
  ls.setRho(0.05);
  ls.setSigma(0.3);
  sd.setLineSearch(&ls);
  sd.setObjective(&rna);
  sd.setMaxIters(10000);
  sd.setEps(epsloc);
  fact.setLocalOptimizer(&sd);
  fact.getOptions() |= Fact::Options::DO_LOCAL_SEARCH;
  dscf.setEps(eps);
  dscf.setObjective(&rna);
  dscg.setEps(eps);
  dscg.setObjective(&rna);
  dscg.setInitialData(&id);
  cdsc.setDim(n);
  fact.addDiscarder(&decomp);
  fact.addDiscarder(&cdsc);
//   fact.addDiscarder(&dscf);
  fact.addDiscarder(&dscg);

  solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
  //solver.setRecord(-0.131665);
  solver.solve();  
  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
}
