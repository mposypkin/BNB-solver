/**
 * Parallel solver for global optimization.
 * Morse potential optimization.
 *
 * 
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file pgo.cpp
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>


#include <util/common/soch.hpp>
#include <kernel/bnblib/msheur2.hpp>
#include <util/aclib/mpiaclib.hpp>
#include <kernel/bnblib/wtraverse.hpp>
#include <kernel/bnblib/simplshed.hpp>
#include <problems/cover/common/go.hpp>
#include <problems/cover/common/gopack.hpp>
#include <kernel/bnblib/simplsuf.hpp>
#include <problems/optlib/fletchsearch.hpp>
#include <problems/optlib/conjgrad.hpp>
#include <kernel/bnblib/basicheurmaster.hpp>
#include <kernel/bnblib/keepbesthm.hpp>
#include <problems/cover/common/contheur.hpp>
#include <problems/semilocal/mbh/basinhop.hpp>
#include <problems/optlib/steepdesc.hpp>
#include <problems/optlib/clevgrad.hpp>
#include <problems/optlib/optsuper.hpp>
#include <problems/optlib/projobjective.hpp>
#include <problems/cover/common/distdivers.hpp>
#include <problems/cover/common/interdiscarder.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <problems/cover/common/spheregenerator.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/optlib/mbfgs.hpp>
#include <problems/molec/common/intmol.hpp>
#include <problems/molec/common/mors.hpp>
#include <problems/molec/common/lenjon.hpp>
#include <problems/molec/common/dzugutov.hpp>
#include <problems/molec/common/cluster.hpp>
#include <problems/molec/common/potprojector.hpp>


template <typename FloatType> class Molec  {

  public:

    void Main (int argc, char *argv[])
    {
      int n, N, *tuple, side;      
      typedef GOFactory<double> Fact;
      typedef IntervalDiscarder<double> IDiscarder;
      IDiscarder intd;
      FloatType scale = 1.;
      FloatType * a, *b;
      const FloatType eps1 = .0001;
      const FloatType eps2 = 0.0001;
      const FloatType hop = 0.8;
      const FloatType dis = 0.7;
      const FloatType rho = 14.;
      const int ns = 4;
      const int poolsize = 1024;
      Decomposer<Fact> decomp;

      Cluster<FloatType> cluster;
      AbstractCommunicator acomm(&argc, &argv);  
      cluster.mNAtoms = atoi (argv[1]);
      cluster.mSpaceD = atoi (argv[2]);
      n = cluster.mSpaceD * cluster.mNAtoms;
      cluster.setDim(n);
      a = new FloatType[n];
      b = new FloatType[n];  
      for(int i = 0; i < n; i ++) {
        a[i] = 0.;
        //b[i] = pow(cluster.mNAtoms, 1./3.) + 1.;
        b[i] = 2.;
        /*
        a[i] = -1.;
        b[i] = 1.;
        */
      }
      BNBDiag::setPriority(BNB_DIAG_MEDIUM);
//       cluster.mPotential = new Mors<FloatType> (14.);
      cluster.mPotential = new LenJon<FloatType> ();;
      IntervalMolec<FloatType> im(cluster.mNAtoms, cluster.mSpaceD, cluster.mPotential);
      im.setDoCheck(false);
      intd.setObjective(&im);
      intd.setEps(eps1);
      GOInitialData<FloatType> id(a, b, eps1, eps1, &im);
      Fact fact;
      fact.addDiscarder(&decomp);
//       fact.getOptions() |= GOFactory<double>::Options::DO_LOCAL_SEARCH;
      intd.setReduceOptions(
                            0     
          |IDiscarder::ReduceOptions::REDUCE_GRAD_ZERO
     //|Fact::ReduceOptions::REDUCE_HANSEN_N
      //|Fact::ReduceOptions::REDUCE_HANSEN_ALL
       //|Fact::ReduceOptions::REDUCE_HANSEN_ALL_ADVANCED
       // |Fact::ReduceOptions::REDUCE_HANSEN_ALLCORNER
       // |Fact::ReduceOptions::REDUCE_HANSEN_BESTCORNER
                           );
      intd.setEvalOptions(
                          0
          | IDiscarder::EvalOptions::EVAL_FUNCTION
       //|Fact::EvalOptions::EVAL_GRAD_SIMPLE
       //|Fact::EvalOptions::EVAL_HANSEN
      //|Fact::EvalOptions::EVAL_POSYP 
       //|Fact::EvalOptions::EVAL_FORM1
                         );
      fact.setInitialData(&id);
//       fact.addDiscarder(&intd);
      
      LipzFuncDiscarder<double> dscc;
      dscc.setEps(eps1);
      dscc.setObjective(&im);

      LipzGradDiscarder<double> dscg;
      dscg.setEps(eps1);
      dscg.setObjective(&im);
//       dscg.setCheckGrad(true);
//       dscg.setCheckGradComp(true);
      dscg.setInitialData(&id);
      
      fact.addDiscarder(&dscc);
      
/*      ConjGrad<double> sd;
      FletchSearch<double> ls;    
      sd.setLineSearch(&ls);
      ls.setEps(eps1 * 0.5);
      ls.setRightEnd(1.);
      ls.setRho(0.2);
      ls.setSigma(0.4);
      sd.setMaxIters(10000);
      sd.setObjective(id.mObj);
      sd.setEps(eps1);*/
        
      MBFGS mms;
      mms.setObjective(&cluster);
      mms.setEps(0.001);
      Basinhop<double> bh;
      bh.setLocalOptimizer(&mms);
      bh.setNumberOfStarts(ns);
      
      fact.setLocalOptimizer(&mms);
      
//      BNBMsHeurSolver < WTraverse < Fact, std::stack < typename Fact::Set > > > solver (&acomm, &fact, 0);
      BNBMsHeurSolver < WTraverse < Fact, std::stack < typename Fact::Set > > > solver (&acomm, &fact, 0/*acomm.getProcessNumber() - 3*/);
      SimpleScheduler sched; 
      sched.setMasterTreshold(0);
      sched.setSlaveTreshold(1000);
      sched.setSlaveChunk(100);
      sched.setMinQueueLength(1);
      sched.setMaxQueueLength(1000);
      solver.setScheduler(&sched);
      solver.setRecordUpdatesRatio(1);
      
//       solver.setNumberOfInitialSolutions(128);
      SimpleSolutionUpdateFilter < Fact > uf;
//       //uf.setAcceptProbability(0.1);
      solver.setSolutionUpdateFilter(&uf);
      
      SphereSolutionGenerator<double> sphere;
      sphere.setInitialData(&id);
      sphere.setRadius(1.);
      solver.setSolutionGenerator(&sphere);
            
      DistDivers<FloatType> md;
      md.setEps(dis);
      
      BNBBasicHeurMaster < Fact > hm;
//       hm.setUpdateMode(BNBBasicHeurMaster< Fact >::UpdateModes::DO_UPDATE_FROM_BNB);
      hm.setDivers(&md);      
      BNBKeepBestHeurMaster < Fact > kbhm;      
      solver.setHeurMaster(&kbhm);
//       solver.setHeurMaster(&hm);            
      ContHeur < Fact > hs;
      hs.setInitialData(&id);      
      hs.setNumberOfStarts(256);
      hs.setAmplitude(hop);
//       hs.setLocalOptimizer(&bh);
      hs.setLocalOptimizer(&mms);
      solver.setHeurSlave(&hs);
      
      solver.solve ();
      acomm.barrier();
      if(acomm.getMyId() == 0)
        printf ("Function minimum is %lf\n", solver.getRecord());    
    }
  private:
    static Potential<FloatType>* mPotential;

};

main(int argc, char* argv[])
{
  Molec<double> mol;
  mol.Main(argc, argv);
}
