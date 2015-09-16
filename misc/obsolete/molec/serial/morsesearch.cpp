
#include <util/common/bnblog.hpp>
#include <util/common/vec.hpp>
#include <util/common/randomstep.hpp>
#include <problems/optlib/grad.hpp>
#include <problems/optlib/simpgrad.hpp>
#include <problems/optlib/newton.hpp>
#include <problems/optlib/mnewton.hpp>
#include <problems/optlib/clevgrad.hpp>
#include <problems/semilocal/mbh/basinhop.hpp>
#include <problems/optlib/steepdesc.hpp>
#include <problems/optlib/goldls.hpp>
#include <problems/optlib/fletchsearch.hpp>
#include <problems/optlib/conjgrad.hpp>
#include <problems/optlib/projobjective.hpp>
//~ #include <problems/optlib/lmbfgs.hpp>
#include <problems/optlib/mbfgs.hpp>
#include <problems/semilocal/mbh/apf.hpp>
#include <problems/molec/common/mors.hpp>
#include <problems/molec/common/lenjon.hpp>
#include <problems/molec/common/dzugutov.hpp>
#include <problems/molec/common/cluster.hpp>
#include <problems/molec/common/potprojector.hpp>
#define NMAX 1000
BNBLog <BNB_DO_LOGGING> logger;
  
main(int argc, char* argv[])
{
  double  *x, *y, *g, v, rho, vbest, eps, ampl, vold;
  int n, M;
  Cluster <double> cluster;  
  BNBLog <BNB_DO_LOGGING> log;
  rho = 14;
  cluster.mNAtoms = atoi (argv[1]);
  cluster.mSpaceD = atoi (argv[2]);
  n = cluster.mNAtoms * cluster.mSpaceD;
  cluster.setDim(n);    
  cluster.mPotential = new Dzugutov<double>();
//   cluster.mPotential =  new Mors<double>(rho);
//   cluster.mPotential =  new LenJon<double>();
//   cluster.mPotential->tabulate(0.5, 2, 0.00001);
  PotProjector potp(cluster.mSpaceD);
//   ProjectedObjective<double> projp(&cluster, &potp);

  x = (double*) malloc(n * sizeof(double)); 
  y = (double*) malloc(n * sizeof(double)); 


  FletchSearch<double> ls;    
  ls.setEps(eps * 0.5);
  ls.setRightEnd(.1);
  ls.setRho(0.2);
  ls.setSigma(0.4);
  ConjGrad<double> cg;
  cg.setLineSearch(&ls);
  cg.setObjective(&cluster);
  cg.setMaxIters(1000);
  cg.setEps(eps);
  /*
  Basinhop<double> bh;
  bh.setLocalOptimizer(&cg);
  */
  
  //~ LMBFGS lms;
  //~ lms.setObjective(&cluster);
  //~ lms.setEps(eps);
  /*
  Basinhop<double> bh;
  bh.setLocalOptimizer(&lms);
  */
  
  MBFGS mms;
  mms.setObjective(&cluster);
  mms.setEps(0.001);
  
  Basinhop<double> bh;
  UniformCubic<double> uc;
  bh.setLocalOptimizer(&mms);
  bh.setRandomStep(&uc);
  bh.setInitStepLength(0.8);
  BoltzmannAcceptProbabilityFunction<double> pf;
  bh.setAcceptProbabilityFunction(&pf);
  ConstSchedule<double> cs;
  bh.setTemperatureSchedule(&cs);
  bh.setRangeSchedule(&cs);
  
  srand(1);
  for (int i = 0; i < n; i ++) {
    y[i] = 1. * (double)rand()/(double)RAND_MAX;
  }    
  int I = 0;
  do {
    vold = 0.;
    int tall, t, tmin = 1000000000, tmax = 0;
    int mni = 256;
    //int mni = 1;
 
    const double mina = 0.8;
    const double h = 0.00;
    const double amp = mina + I * h;
    I ++;
    
    srand(I);
    tall = clock();
    bh.setNumberOfStarts(mni);
    pf.setParam(amp);
//     bp.pregenerate(mni);
    for (int i = 0; i < n; i ++) {
      x[i] = y[i];
    }    
    bool rv;
    t = clock();
    if(mni == 1)
      rv = mms.search(x, &v);
    else
      rv = bh.search(x, &v);
    t = clock() - t;
    if(t < tmin)
      tmin = t;
    if(t > tmax)
      tmax = t;            
    if(!rv)
      printf("FAILED!\n");
    log << amp << ": v = " <<  v << "\n";
    log.write();
    tall = clock() - tall;
    log << " time: " << tall << "\n";
    log.write();
  } while(true);
}
