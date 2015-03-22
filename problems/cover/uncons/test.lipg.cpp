#include <problems/cover/common/go.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/segvecdecomp.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <kernel/bnblib/seq.hpp>
#include "rosenbrokn.hpp"
#include "booth.hpp"
#include "sqsum.hpp"
#include "barkalov.hpp"
#include "rastrlip.hpp"
#include "grien.hpp"


main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  typedef std::priority_queue <Fact::Set>  TQ;
  
  int n;
  double eps = .001;
  bool rv = true;
  GOInitialData<double> id;
  n = 2;
  Sqsum obj(n);
  id.mObj = &obj;
  id.mA = (double*) malloc(n * sizeof(double));
  id.mB = (double*) malloc(n * sizeof(double));
  id.mA[0] = -10.;
  id.mA[1] = -10.;
  id.mB[0] = 50.;
  id.mB[1] = 10.;
  LipzGradDiscarder<double> dscg;
  Fact fact;
  SegVecDecomp<Fact> decomp;
  dscg.setEps(eps);
  dscg.setObjective(&obj);
  dscg.setInitialData(&id);
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_GRAD;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_GRAD_BALL_CUT;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
  dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_UNCONS_BALL_CUT;

  
  fact.setInitialData(&id);
  fact.addDiscarder(&dscg);
  fact.addDiscarder(&decomp);
  Solver solvertry(&fact);
  solvertry.setMST(1);
  solvertry.solve();
  TQ tq = solvertry.getCandidateProblemsList();
  if(tq.size() != 2) {
    BNB_ERROR_REPORT("wrong number of boxes (should be two)\n");
  }
  Fact::Set s = tq.top();
  double er = BNBABS(s.mA[0] - (-10.));
  er +=  BNBABS(s.mA[1] - (-10.0));
  er +=  BNBABS(s.mB[0] - 2.679550);
  er +=  BNBABS(s.mB[1] - 0.0);
  tq.pop();
  s = tq.top();
  er += BNBABS(s.mA[0] - (-10.));
  er +=  BNBABS(s.mA[1] - (.0));
  er +=  BNBABS(s.mB[0] - 2.679550);
  er +=  BNBABS(s.mB[1] - 10.0);
  tq.pop();
  if(er > 0.001) {
    BNB_ERROR_REPORT("wrong box\n");
  } 

  Solver solver(&fact);
 // solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
  solver.solve();  
  Fact::Solution sol = solver.getSolutionContainer()->top();
//  printf("Sol val: %lf\n", sol.getValue());
  if(sol.getValue() > eps) {
    BNB_ERROR_REPORT("wrong solution value\n");
  }
  return 0;
}
