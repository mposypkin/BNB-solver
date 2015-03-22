#include "rna.hpp"
#include <problems/optlib/simpgrad.hpp>
#include <problems/optlib/mbasinhop.hpp>
#include "rnagen.hpp"
#include <problems/optlib/fletchsearch.hpp>
#include <problems/optlib/steepdesc.hpp>
#include "sultan.hpp"

void init(int n, double* h, double* kap, double* x)
{
  for(int i = 0; i < n; i ++) {
    for(int j = 0; j < n; j ++) {
      double v;
      if(i == j)
        v = 1.;
      else 
        v = 0;
      h[i * n + j] = v;
    }
    //(*kap)[i] = .5 * ((double) rand()) / ((double) RAND_MAX);
    //(*kap)[i] = .1 + i / 10.;
    //kap[i] = 1.;
    kap[i] = 2.;
    
    x[i] = ((double) rand()) / ((double) RAND_MAX);
  }
}


void allocArrays(int n, double** h, double** kap, double** x)
{
  *h = (double*) calloc(n * n, sizeof(double));
  *kap = (double*) calloc(n, sizeof(double));
  *x = (double*) calloc(n, sizeof(double));
}

main(int argc, char* argv[])
{
  RNABase<double> rnabase;
  RNA rna;
  SimpGrad<double> simg; 
  int n;
  int ns;
  double *h, *kap, *x, v, ampl, eps;

  eps = 0.01;
  ampl = 1.;
/*  init(n, h, kap, x);*/
//   RnaGenerator::generate("actg", n, rnabase);
  //initSultan2(n, rna);
  initSultan(n, rnabase);
  ns = n * n * n;
  allocArrays(n, &h, &kap, &x);
  MatrUtils::print(n, n, h);
  VecUtils::vecPrint(n, kap);
  x[0] = 1.;
  x[1] = 0.1;
  x[2] = 0.1;
  printf("%lf\n", rna.func(x));
  simg.setEps(eps);
  simg.setMu(0.000001);
  simg.setObjective(&rna);
//  simg.search(x, &v);
  SteepDesc<double> sd;
  FletchSearch<double> ls;
  ls.setEps(0.5 * eps);
  ls.setRightEnd(24.);
  ls.setRho(0.05);
  ls.setSigma(0.3);

  sd.setLineSearch(&ls);
  sd.setObjective(&rna);
  sd.setMaxIters(10000);
  sd.setEps(eps);
  sd.search(x, &v);
  printf("after grad %lf\n",   rna.func(x));
  VecUtils::vecPrint(n, x);
  MBasinhop<double> mbh;
  mbh.setLocalOptimizer(&sd);
  mbh.setNumberOfStarts(ns);
  mbh.setAmplitude(ampl);
  mbh.setDoMultistart(true);
  mbh.search(x, &v);
  mbh.printPool();
}
