#include <stdlib.h>
#include <problems/go/go.hpp>
#include <problems/go/decomposer.hpp>
#include <problems/go/lipzfdiscarder.hpp>
#include <problems/go/lipzgdiscarder.hpp>
#include <kernel/bnblib/seq.hpp>
#include "xsq.hpp"
#include "poli4.hpp"
#include "trig.hpp"
#include "polynom.hpp"
#include <problems/optlib/fletchsearch.hpp>
#include <problems/optlib/conjgrad.hpp>
#include "sinus.hpp"
#include "polytrig.hpp"
#include "linsin.hpp"
#include "pinter.hpp"

void init(LipzObjective<double>& obj, GOInitialData<double>& id, double ampl)
{
  id.mObj = &obj;
  id.mA = (double*) malloc(sizeof(double));
  id.mB = (double*) malloc(sizeof(double));
  id.mA[0] = -ampl;
  id.mB[0] = ampl;
}

void fillPoly(int n, double* coef)
{
  double A = 3.;
  double scale = .25;
  double *ncoef = (double*) malloc((n + 1) * sizeof(double));
  for(int i = 0; i <= n; i ++) {
    coef[i] = 0.;
  }
  coef[0] = 1.;
  coef[1] = -1.;
  for(int i = 2; i <= n; i ++) {
    double x = ((double)rand() / (double)RAND_MAX - 0.5) * A * 2.;
    printf("root = %lf\n", x);
    for(int k = 0; k <= i; k ++) {
      ncoef[k] = 0.;
    }
    for(int k = 0; k <= i; k ++) {
      if(k != i) 
        ncoef[k] += coef[k];
      if(k != 0)
        ncoef[k] -= coef[k - 1] * x;
    }
    for(int k = 0; k <= i; k ++) {
      coef[k] = ncoef[k];
    }
  }
  for(int i = 0; i <= n; i ++) {
    coef[i] *= scale;
  }
  printf("POLY:\n");
  for(int i = 0; i <= n; i ++) {
    printf("%lf * x**%d", coef[i], n - i);
    if(i != n)
      printf(" + ");
    else 
      printf("\n");
  }
  free(ncoef);
}

void fillPolySymm(int n, double* coef)
{
  double *ncoef = (double*) malloc((n + 1) * sizeof(double));
  double scale = pow(0.1, n/4 + 2);
  printf("scale = %lf\n", scale);
  for(int i = 0; i <= n; i ++) {
    coef[i] = 0.;
  }
  coef[0] = 1.;
  coef[1] = -1.;
  for(int i = 2; i <= n; i ++) {
    double x;
    if(i % 2 == 0) {
      x = -i/2;
    } else {
      x = (i + 1) / 2;
    }
    printf("root = %lf\n", x);
    for(int k = 0; k <= i; k ++) {
      ncoef[k] = 0.;
    }
    for(int k = 0; k <= i; k ++) {
      if(k != i) 
        ncoef[k] += coef[k];
      if(k != 0)
        ncoef[k] -= coef[k - 1] * x;
    }
    for(int k = 0; k <= i; k ++) {
      coef[k] = ncoef[k];
    }
  }
  for(int i = 0; i <= n; i ++) {
    coef[i] *= scale;
  }
  printf("POLY:\n");
  for(int i = 0; i <= n; i ++) {
    printf("%lf * x**%d", coef[i], n - i);
    if(i != n)
      printf(" + ");
    else 
      printf("\n");
  }
  free(ncoef);
}


main(int argc, char* argv[])
{
  typedef GOFactory < double > Fact;
  typedef WTraverse < Fact > Traverse;
  typedef BNBSeqSolver < Traverse > Solver;
  
  int n;
  double eps, epsLoc = 0.001;
  BNB_ASSERT(argc >= 3);
  eps = atof(argv[1]);
  srand(atoi(argv[2]));

//   const int deg = 4;
//   double coef[deg + 1] = {1., -10., 35., -50., 24.};
//   double coef[deg + 1] = {24., -142., 303., -276., 3.};
//   const int deg = 6;
//   double coef[deg + 1] = {1., 0., -15., 0., 27., 0., 250.};
//   Polynom obj(deg, coef);
  
//  fillPoly(deg, coef);
 // fillPolySymm(deg, coef);
//   fillPolySergeev(coef);
//    Sinus obj;
//   Linsin obj;
//   Polytrig obj;
//   Poli4 obj;
  
  const int dim = 1;
  double coe[dim * 2] = {10000.,1.};
  Pinter  obj(dim, 0.00, coe);
  double X = 0.;
  printf("pinter: %s, %lf\n", obj.toString().c_str(), obj.func(&X)); 
   
  GOInitialData<double> id;  
  Decomposer<Fact> decomp;
  LipzFuncDiscarder<double> dscf;
  LipzGradDiscarder<double> dscg;
  Fact fact;
  dscf.setEps(eps);
  dscf.setObjective(&obj);
  dscg.setEps(eps);
  dscg.setObjective(&obj);
  dscg.setInitialData(&id);
//   dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD;
//   dscg.getOptions() &= ~LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;

  init(obj, id, 10.);
  fact.setInitialData(&id);
//   fact.addDiscarder(&decomp);
//    fact.addDiscarder(&dscf);
   fact.addDiscarder(&dscg);
  
  ConjGrad<double> sd;
  FletchSearch<double> ls;    
  sd.setLineSearch(&ls);
  ls.setEps(epsLoc * 0.5);
  ls.setRightEnd(.1);
  ls.setRho(0.1);
  ls.setSigma(0.4);
  sd.setMaxIters(10000);
  sd.setObjective(id.mObj);
  sd.setEps(epsLoc);
  fact.setLocalOptimizer(&sd);
//   fact.getOptions() |= GOFactory < double >::Options::DO_LOCAL_SEARCH;
//   fact.getOptions() &= ~GOFactory < double >::Options::STD_SOLTUION_GENERATION;
  Solver solver(&fact);
  solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
//   solver.setRecord(0.);
  solver.solve();  
  printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
}
