#ifndef __PARMBH_HPP__
#define __PARMBH_HPP__

#include <stdio.h>
#include <string>
#include <kernel/bnblib/bnb.hpp>
#include <util/aclib/aclib.hpp>
#include <kernel/bnblib/wtraverse.hpp>
#include <util/common/bnberrcheck.hpp>
#include <kernel/bnblib/msheur2.hpp>
#include <problems/cover/common/go.hpp>
#include <problems/cover/common/gopack.hpp>
#include <problems/cover/common/distdivers.hpp>
#include <kernel/bnblib/basicheurmaster.hpp>
#include <kernel/bnblib/keepbesthm.hpp>
#include <problems/cover/common/contheur.hpp>
#include <problems/optlib/basinhop.hpp>
#include <util/xml/xmlpars.hpp>
#include "pmbhfactory.hpp"
#include "locoptimfactory.hpp"
#include <problems/optlib/boxpert.hpp>
//#include <apps/crypto/cryptosolver.hpp>

class ParmbhFactory {
  public:

    /**
      * Create Parallel Monotonic Basin Hopping Solver
      *
      * @param pd ProblemData
      * @param gac Global Abstract Communicator
      * @param ac Abstract Communicator
      * @param sdata solver data
      * @return newly created BNBSolver
      */
  static BNBSolver* createSolver(BNBProblemData* pd, AbstractCommunicator* gac, AbstractCommunicator* ac, std::string sdata)
  {
    int bp = 0;
    typedef GOFactory<double> Fact;
    typedef WTraverse < Fact, std::stack < Fact::Set > > WT;
    Fact *fact;
    GOInitialData < double > *id;
    BNBMsHeurSolver < WT > *ss;
    XMLPars pars;
    id = dynamic_cast<GOInitialData < double > *> (pd);
    fact = new Fact();
    fact->setInitialData(id);
    ss = new BNBMsHeurSolver < WT > (ac, fact, 0);
    ss->setGlobalCommunicator(gac);
    int poolsz;
    int nhop;
    double hop;
    double dist;
    std::string locsearch;
    std::map<std::string, std::string> tls;
    pars.parse(sdata, tls);

    if(tls.find(PMBH_NHOPS_TAG) == tls.end()) {
      BNB_ERROR_REPORT("Number of hops not set");
    } else
      nhop = atoi(tls[PMBH_NHOPS_TAG].c_str());

    if(tls.find(PMBH_HOP_TAG) == tls.end()) {
      BNB_ERROR_REPORT("Hop size not set");
    } else
      hop = atof(tls[PMBH_HOP_TAG].c_str());

    if(tls.find(PMBH_LS_TAG) == tls.end()) {
      BNB_ERROR_REPORT("Local search not set");
    } else
      locsearch =  tls[PMBH_LS_TAG];

    ContHeur < Fact > *hs;
    hs = new ContHeur < Fact >();
    hs->setAmplitude(hop);
    if(tls.find(PMBH_PERT_TAG) != tls.end()) {
      if(tls[PMBH_PERT_TAG] == PMBH_DIVIDE_TAG)
	hs->setDivideCube(true);
      else if(tls[PMBH_PERT_TAG] == PMBH_PREGEN_TAG) {
	const int n = id->mObj->getDim();
	const int nslaves = ac->getProcessNumber() - 2;
	const int nos = nhop / nslaves;
	const int myid = ac->getMyId();
	const int slave0 = 2;
	BoxPertruber<double>* bp = new BoxPertruber<double>(hop, n);
	if(myid == slave0) {
	  for(int i = 1; i < nslaves; i ++) {
	    double *p;
	    bp->pregenerate(nos);
	    p = bp->getGen();
	    ac->packArray(p, n * nos);
	    ac->send(slave0 + i);
	  }
	  bp->pregenerate(nos);
	} else if(myid > slave0) {
	  double *p;
	  ac->recv(slave0);
	  bp->pregenerate(nos);
	  ac->unpackArray(bp->getGen(), n * nos);
	}
	hs->setPertruber(bp);
      }
    }

    BNBKeepBestHeurMaster < Fact > *kbhm = new BNBKeepBestHeurMaster < Fact >();
    ss->setHeurMaster(kbhm);
    ss->setHeurSlave(hs);
    LocalOptimizer<double>* lo;
    lo = LocalOptimizerFactory::create(locsearch, id->mObj);
    hs->setNumberOfStarts(nhop / (ac->getProcessNumber() - 2));
    hs->setInitialData(id);
    hs->setLocalOptimizer(lo);
    return ss;
  }
};
#endif
