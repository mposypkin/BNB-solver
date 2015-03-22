#include <problems/cover/common/go.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <kernel/bnblib/seq.hpp>
#include "linsum.hpp"
#include "linsumlag.hpp"
#include "conssm.hpp"
#include "sphereproj.hpp"
#include "spheredisc.hpp"
#include "dna.hpp"
#include "dnalag.hpp"
#include "sult.hpp"

void init(int n, LipzObjective<double>* obj, GOInitialData<double>& id)
{
  id.mObj = obj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  for(int i = 0; i < n; i ++) {
    id.mA[i] = -1;
    id.mB[i] = 1;
  }
  id.mA[n - 1] = -12.;
  id.mB[n - 1] = -11.;
}

main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  
  int m = 4;
  int n = m + 1;
  double eps = 0.5;
  
//   Linsum lsobj(n);
//   LinsumLag clsobj(n);
  DNA* dna = initSultan5(m, n);
  DNALag cdna(dna);  

//    LipzObjective<double> *obj = &lsobj;
//   LipzObjective<double> *cobj = &clsobj;
  LipzObjective<double> *obj = dna;
  LipzObjective<double> *cobj = &cdna;
  GOInitialData<double> id;
  Decomposer<Fact> decomp;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  LipzGradDiscarder<double> dsccons;
  SphereDiscarder sdisc(m);
  ConsSolutionMaker solmaker;
  SphereProjector proj(m);
  Fact fact;
  dscf.setEps(eps);
  dscf.setObjective(obj);
  dscg.setEps(eps);
  dscg.setObjective(obj);
  dscg.setInitialData(&id);
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
  dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::UPDATE_RECORD;
  dsccons.setObjective(cobj);
  dsccons.setInitialData(&id);
  dsccons.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_LB;
//   dsccons.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
  init(n, obj, id);
  fact.setInitialData(&id);
  fact.addDiscarder(&decomp);
  fact.addDiscarder(&sdisc);
  fact.addDiscarder(&dsccons);
//   fact.addDiscarder(&dscf);
  fact.addDiscarder(&dscg);
  solmaker.setObjective(obj);
  solmaker.setProjector(&proj);
  fact.setSolutionMaker(&solmaker);
  Solver solver(&fact);
  solver.setDebugOptions(
      Solver::Options::PRINT_RESULTING_STAT 
//       | Solver::Options::PRINT_STATE
                        );
//  solver.Traverse::setDebugOptions(Traverse::Options::PRINT_STEP);
//   solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT | Solver::Options::PRINT_INITIAL_VAL);
  //solver.setRecord(n - 1);
  solver.solve();  
  if(solver.getSolutionContainer()->empty())
    printf("Empty container\n");
  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
  
}
