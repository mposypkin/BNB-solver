#include "rosenbrok.hpp"
#include "mccormic.hpp"
#include "matjas.hpp"
#include "rastrigin.hpp"
#include "branin.hpp"
#include "hump.hpp"
#include "shubert.hpp"
#include <kernel/bnblib/seq.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/interdiscarder.hpp>

main(int argc, char* argv[])
{
  int dim;
  double *a, *b, eps, d;  
  Rosenbrok <double> rbrok;
  McCormic <double> mccormic;
  Matjas <double> matjas;
  Rastrigin <double> rastr;
  Branin <double> branin;
  Hump<double> hump;
  Shubert<double> shubert;
  IntervalObjective<double>* obj;
  typedef IntervalDiscarder<double> IDiscarder;
  typedef GOFactory <double> Fact;
  Decomposer<Fact> decomp;
  IDiscarder intd;
  //typedef SimpleIntervalFactory<double> Fact;
  typedef BNBSeqSolver < WTraverse < Fact > > Solver;
  dim = 2;
  eps = 0.00001;
  obj = &hump;
  obj->setDim(dim);
  intd.setObjective(obj);

  a = (double*)malloc(dim * sizeof(double));
  b = (double*)malloc(dim * sizeof(double));
  obj->fillBounds(dim, a, b);
  GOInitialData<double> id(a, b, eps, eps, obj);
  Fact sf;
  //sf.setReduceFactor(2);
  Solver solver (&sf);  
  intd.setReduceOptions(
      0
     //|Fact::ReduceOptions::REDUCE_GRAD_ZERO
     //|Fact::ReduceOptions::REDUCE_HANSEN_N
      |IDiscarder::ReduceOptions::REDUCE_HANSEN_ALL
    //|Fact::ReduceOptions::REDUCE_HANSEN_ALL_ADVANCED
    //|Fact::ReduceOptions::REDUCE_HANSEN_ALLCORNER
    //|Fact::ReduceOptions::REDUCE_HANSEN_BESTCORNER
                     );
  intd.setEvalOptions(
      0
      | IDiscarder::EvalOptions::EVAL_FUNCTION
       //|Fact::EvalOptions::EVAL_GRAD_SIMPLE
      |IDiscarder::EvalOptions::EVAL_HANSEN
      //|Fact::EvalOptions::EVAL_POSYP 
       //|Fact::EvalOptions::EVAL_FORM1
                   );
  intd.setEps(eps);
  sf.setInitialData(&id);
  sf.addDiscarder(&decomp);
  sf.addDiscarder(&intd);
  //solver.setRemccormiccord(-1.03163);
  //solver.setRecord(-186.731);
  //solver.setRecord(0.);
  //solver.setRecord(-1.91322);
  solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT /*| Solver::Options::PRINT_STATE*/);
  solver.solve();

  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
}

