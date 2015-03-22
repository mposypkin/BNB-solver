#ifndef __MULTISTART_HPP__
#define __MULTISTART_HPP__

#include <stdio.h>
#include <string>
#include <kernel/bnblib/bnb.hpp>
#include <util/common/bnberrcheck.hpp>
#include <kernel/bnblib/msheur2.hpp>
#include <problems/cover/common/go.hpp>
#include <problems/cover/common/gopack.hpp>
#include <kernel/bnblib/basicheurmaster.hpp>
#include <problems/cover/common/contheur.hpp>
#include <util/xml/xmlpars.hpp>
#include "locoptimfactory.hpp"


/**
 * Multistart tags
 */

/**
 * Number of local (at each processor)
 * steps
 */
#define MULTISTART_NLS_TAG "nls"

/**
 * Number of initial solutions
 */
#define MULTISTART_NIS_TAG "nis"

/**
 * Amplitude at each point
 */
#define MULTISTART_AMPL_TAG "ampl"

/**
 * Local search descriptor
 */
#define MULTISTART_LS_TAG "localsearch"


class MultistartFactory {
  public:

    /**
     * Create Parallel Multistart Solver
     *
     * @param pd ProblemData
     * @param gac Global Abstract Communicator
     * @param ac Abstract Communicator
     * @param sdata solver data
     * @return newly created BNBSolver
     */
    static BNBSolver* createSolver(BNBProblemData* pd, AbstractCommunicator* gac, AbstractCommunicator* ac, std::string sdata)
    {
      int nls, nis;
      double ampl;
      GOInitialData < double > *id;
      XMLPars pars;
      typedef GOFactory<double> Fact;
      Fact* fact;
      typedef WTraverse < Fact, std::stack < Fact::Set > > WT;
      BNBMsHeurSolver < WT > *ss;

      std::string locsearch;
      std::map<std::string, std::string> tls;
      pars.parse(sdata, tls);
      id = dynamic_cast<GOInitialData < double > *> (pd);
      fact = new Fact();
      fact->setInitialData(id);
      ss = new BNBMsHeurSolver < WT > (ac, fact, 0);
      ss->setGlobalCommunicator(gac);

      nis = 1;
      nls = 1;
      ampl = 0;
      if(tls.find(MULTISTART_LS_TAG) == tls.end()) {
	BNB_ERROR_REPORT("Local search not set");
      } else {
	locsearch =  tls[MULTISTART_LS_TAG];
      }
      BNBBasicHeurMaster < Fact >* hm;
      hm = new BNBBasicHeurMaster < Fact >();
      hm->setUpdateMode(
	  BNBBasicHeurMaster < Fact >::UpdateModes::NO_UPDATE |
	  BNBBasicHeurMaster < Fact >::UpdateModes::DO_UPDATE_FROM_MASTER);
      hm->setSendToAll(false);
      ss->setHeurMaster(hm);
      ss->setNumberOfInitialSolutions(nis);
      ContHeur < Fact > *hs;
      hs = new ContHeur < Fact >();
      hs->setAmplitude(ampl);
      hs->setInitialData(id);
      hs->setNumberOfStarts(nls);
      ss->setHeurSlave(hs);
      LocalOptimizer<double>* lo;
      lo = LocalOptimizerFactory::create(locsearch, id->mObj);
      hs->setLocalOptimizer(lo);
      return ss;
    }
};


#endif
