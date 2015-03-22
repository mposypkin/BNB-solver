#ifndef __SLOCFACTORY_HPP__
#define __SLOCFACTORY_HPP__

#include <string>
#include <map>
#include <util/xml/xmlpars.hpp>
#include <problems/cover/common/go.hpp>
#include "locoptimfactory.hpp"
#include <problems/cover/common/slocsolver.hpp>


/**
  * Factory for creating serial local search solver
  */

#define SLOC_LS_TAG "localsearch"

#define DELTA_TAG "delta"

class SLocSolverFactory {
  public:
    static SLocSolver* createSolver(BNBProblemData* pd, const std::string &sdata) 
    {
      std::map<std::string, std::string> tls;
      std::string sheur, locsearch;
      XMLPars pars;
      double delta;
      pars.parse(sdata, tls);
      if(tls.find(SLOC_LS_TAG) == tls.end()) {
	BNB_ERROR_REPORT("Local search not set");
      } else {
	locsearch =  tls[SLOC_LS_TAG];
      }
      if(tls.find(DELTA_TAG) == tls.end()) {
          delta = 0.;
      } else {
          delta = atof(tls[DELTA_TAG].c_str());
      }

      GOInitialData < double > *id;
      id = dynamic_cast<GOInitialData < double > *> (pd);
      LocalOptimizer<double>* lo;
      lo = LocalOptimizerFactory::create(locsearch, id->mObj);
      SLocSolver* slocsolver = new SLocSolver();
      slocsolver->setLocalOptimizer(lo);
      slocsolver->setDelta(delta);
      return slocsolver;
    }
};
#endif
