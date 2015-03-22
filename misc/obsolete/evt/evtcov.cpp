#include <stdio.h>
#include <stdlib.h>
#include <problems/multigo/multigo.hpp>
#include "ccc.hpp"
#include "evt.hpp"
#include "evtm.hpp"
#include <kernel/bnblib/ntraverse.hpp>
#include <kernel/bnblib/seq.hpp>
#include <problems/go/decomposer.hpp>
#include <problems/multigo/multicovfdisc.hpp>
#include <problems/multigo/multicovgdisc.hpp>

main()
{
  int n;
  int m;
  
#if 0  
  double a[2] = {-0.5, -0.5};
  double b[2] = {0.5, 0.5};
  double eps[2] = {0.01, 0.01};
  Ccc cf;
#endif

#if 1 
  double a[1] = {0.};
  double b[1] = {2.};
  double eps[2] = {0.01, 0.01};
  Evt cf;
#endif

#if 0 
  double a[2] = {0., 0.};
  double b[2] = {2., 2.};
  double eps[2] = {0.1, 0.1};
  Evtm cf;
#endif

  MGOInitialData mgodata(a, b, eps, &cf);
  MGOFactory fact;
  fact.setInitialData(&mgodata);
  Decomposer<MGOFactory> decomp;
  fact.addDiscarder(&decomp);
  multicovFDiscarder fdisc;
  fdisc.setObjective(&cf);
  fdisc.setEps(eps);
  multicovGDiscarder gdisc;
  gdisc.setObjective(&cf);
  gdisc.setEps(eps);
//  fact.addDiscarder(&fdisc);
  fact.addDiscarder(&gdisc);
  typedef BNBSeqSolver < NTraverse <MGOFactory> > Solver;
  Solver solver(&fact);
  solver.setDebugOptions(Solver::Options::PRINT_STATE);
  solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
  solver.solve();
  printf("Solution:\n %s\n", solver.getIncumbent().toString().c_str());
}
