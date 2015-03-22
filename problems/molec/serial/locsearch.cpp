/**
 * The program to test gradient methods
 */

#include <queue>
#include <string>
#include <sstream>

#include <util/common/bnblog.hpp>
#include <util/common/vec.hpp>
#include <problems/optlib/grad.hpp>
#include <problems/optlib/simpgrad.hpp>
#include <problems/optlib/newton.hpp>
#include <problems/optlib/newtonls.hpp>
#include <problems/optlib/mnewton.hpp>
#include <problems/optlib/clevgrad.hpp>
#include <problems/optlib/neigh.hpp>
#include <problems/optlib/steepdesc.hpp>
#include <problems/optlib/goldls.hpp>
#include <problems/optlib/fletchsearch.hpp>
#include <problems/optlib/conjgrad.hpp>
#include <problems/optlib/optsuper.hpp>
#include <problems/optlib/projobjective.hpp>
#include <problems/optlib/lmbfgs.hpp>
#include <problems/molec/common/mors.hpp>
#include <problems/molec/common/lenjon.hpp>
#include <problems/molec/common/cluster.hpp>
#include <problems/molec/common/potprojector.hpp>

  BNBLog <BNB_DO_LOGGING> logger;
  
  struct AlgoRec {
    int time;
    std::string name;
    
    bool operator < (const AlgoRec & s) const
    {
      return time > s.time;
    }
    
    std::string toString() const
    {
      std::ostringstream os;
      os << name << ", time: " << time << "\n";
      return os.str();
    }
  };
  
  
  void init(int N, int D, double* x)
  {
    int n, k;
    n = D * N;
    double A = 1.;
    /*
    for(int i = 0; i < N; i ++) {
      x[i] = i;
    }
    
    for(int i = 0; i < N; i ++) {
      k = i % D;
      for(int l = 0;l < D; l ++) {
        x[i * D + l] = 0;
      }
      x[i * D + k] = i / D + i;
    }
    */
    /*
    x[0] = 0;
    x[1] = .4;
    */
    
    //x[2] = 3;
    
    for(int i = 0; i < n; i ++) {
      x[i] = A * (((double)rand())/ RAND_MAX);
    }
    //for(int i = 0; i < n; i ++)
    //  x[i] = i;
  }
  
   main(int argc, char* argv[])
  {
    double  *x0, *x, *g, v ,eps;
    int n, t;
    std::priority_queue<AlgoRec> algos;
    Cluster <double> cluster;
    double scale = 1.;
    //cluster.mPotential =  new LenJon<double>();
    cluster.mPotential =  new Mors<double>(14);
//    cluster.mPotential->setScale(1./scale);
//      cluster.mPotential->tabulate(0.01, 15., 3);
    cluster.mNAtoms = atoi (argv[1]);
    cluster.mSpaceD = atoi (argv[2]);
    eps = atof(argv[3]);
    if(argc == 5)
      srand(atoi(argv[4]));
    else
      srand(1);
    n = cluster.mNAtoms * cluster.mSpaceD;
    cluster.setDim(n);    
    x0 = (double*) malloc(n * sizeof(double));
    x = (double*) malloc(n * sizeof(double));        
    g = (double*) malloc(n * sizeof(double));        
    init(cluster.mNAtoms, cluster.mSpaceD, x0);
    PotProjector potp(cluster.mSpaceD);
//     ProjectedObjective<double> projp(&cluster, &potp);

    t = clock();
    for(int i = 0; i < 10000; i ++)
    {
      cluster.grad(x0, g);
    }
    t = clock() - t;
    printf("t grad = %d\n", t);
    
    for(int i = 0; i < n; i ++)
      x[i] = x0[i];
    logger << "Start\n";
    logger.write();
    FletchSearch<double> ls;    
    ConjGrad<double> sd;
    sd.setLineSearch(&ls);
    ls.setEps(0.5 * eps);
    sd.setObjective(&cluster);
    sd.setMaxIters(10000);
    sd.setEps(eps);
//           sd.setObjective(&projp);
    
    for(double re = .1 * scale; re < 1.* scale; re += .2 * scale) {
      for(double sig = 0.1; sig < 0.5; sig += 0.1) {
        for(double ro = 0.1; ro < sig; ro += 0.1) {
          for(int i = 0; i < n; i ++)
            x[i] = x0[i];
          ls.setRightEnd(re);
          ls.setRho(ro);
          ls.setSigma(sig);
          t = clock();
          if(sd.search(x, &v)) {
            std::ostringstream os;
            AlgoRec ar;
            t = clock() - t;
            os << "Conj Gradient (" ;
            os << "re = " << re << ",";
            os << "ro = " << ro << ",";
            os << "sigma = " << sig << "):";
            os << " v = " << v;
            ar.time = t;
            ar.name = os.str();
            algos.push(ar);
            logger << ar.toString();
            logger << "(v = " << v << ")\n";
            logger.write();            
//        VecUtils::vecPrint(n, x);
          } else {
            logger << "CG Failed to improve\n";
            logger.write();
          }          
        }
      }
    }

 
    ls.setRightEnd(0.1);
    ls.setRho(0.3);
    ls.setSigma(0.4);
/*
    for(int i = 0; i < n; i ++)
      x[i] = x0[i];
    SimpGrad<double> cg;
    cg.setObjective(&cluster) ;
    cg.setEps(eps);
    cg.setMaxIters(256);
    cg.setMu(.1);
    //cg.setProjector(&mproj);
    t = clock();
    if(cg.search(x, &v)) {
      std::ostringstream os;
      AlgoRec ar;
      t = clock() - t;
      os << "Simp Gradient";
      ar.time = t;
      ar.name = os.str();
      algos.push(ar);
      logger << ar.toString();
      logger << "(v = " << v << ")\n";
      logger.write();
//       VecUtils::vecPrint(n, x);
    } else {
      logger << "Failed to improve\n";
      logger.write();
    }
    
    for(int i = 0; i < n; i ++)
      x[i] = x0[i];
    OptSuper<double> optsu;
    cg.setEps(eps);
    optsu.pushBack(&cg);
    optsu.pushBack(&sd);
    t = clock();
    if(optsu.search(x, &v)) {
      std::ostringstream os;
      AlgoRec ar;
      t = clock() - t;
      os << "Simple Gradient + cg";
      ar.time = t;
      ar.name = os.str();
      algos.push(ar);
      logger << ar.toString();
      logger << "(v = " << v << ")\n";
      logger.write();
//       VecUtils::vecPrint(n, x);
    } else {
      logger << "Failed to improve\n";
      logger.write();
    }
  */  
    while(!algos.empty()){
      logger << algos.top().toString();
      algos.pop();
      logger.write();
    }
      
/*
    NewtonLS<double> nt;
    for(int i = 0; i < n; i ++)
      x[i] = x0[i];
    nt.setObjective(&cluster);
    nt.setEps(0.01);
    nt.setLineSearch(&ls);
    if(nt.search(x, &v)) {
      logger << "After Newton:" << v;
      logger.write();
      VecUtils::vecPrint(n, x);
    } else {
      logger << "Failed to improve\n";
      logger.write();
    }
  
    */
  }

