#ifndef __LOCOPTIMFACTORY_HPP__
#define __LOCOPTIMFACTORY_HPP__

#include <problems/optlib/locoptim.hpp>
#include <problems/optlib/conjgrad.hpp>
#include <problems/optlib/mbfgs.hpp>
#include <problems/semilocal/mbh/basinhop.hpp>
//~ #include <problems/semilocal/mbh/boxpert.hpp>
#include <problems/semilocal/mbh/apf.hpp>
#include "linesearchfactory.hpp"

#define CONJGRAD_TAG "conjgrad"

#define CONJGRAD_MI_TAG "mi"

#define CONJGRAD_EPS_TAG "eps"

#define CONJGRAD_LINESEARCH_TAG "linesearch"

#define BASIN_HOP_TAG "basinhop"

#define BASIN_HOP_NHOP_TAG "nhop"

#define BASIN_HOP_AMPL_TAG "ampl"

#define BASIN_HOP_LOCSEARCH_TAG "localsearch"

#define LBFGS_TAG "lbfgs"


class LocalOptimizerFactory {
  public:
    
    static LocalOptimizer<double>* create(std::string descript, Objective<double>* obj)
    {
      XMLPars pars;
      LocalOptimizer<double>* locs;
      printf("LOCAL SEARCH: %s\n", descript.c_str());
      std::map<std::string, std::string> tls;
      std::string lsname, lsdata;
      pars.parse(descript, tls);            
      lsname = tls.begin()->first;
      lsdata = tls.begin()->second;
      if(lsname == CONJGRAD_TAG) {        
        int mi;
        double eps;
        std::map<std::string, std::string> tls;
        pars.parse(lsdata, tls);                    
        if(tls.find(CONJGRAD_LINESEARCH_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Line search not set");
        } 
        if(tls.find(CONJGRAD_EPS_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Precision not set");
        } else{
          eps = atof(tls[CONJGRAD_EPS_TAG].c_str());
        }
        if(tls.find(CONJGRAD_MI_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Maximal iterations (mi) not set");
        } else{
          mi = atoi(tls[CONJGRAD_MI_TAG].c_str());
        }
        
        ConjGrad<double>* sd = new ConjGrad<double>();
        LineSearch<double> *ls;    
        ls = LineSearchFactory::createLineSearch(tls[CONJGRAD_LINESEARCH_TAG]);        
        sd->setLineSearch(ls);
        sd->setMaxIters(mi);
        sd->setObjective(obj);
        sd->setEps(eps);
        locs = sd;        
      } else if(lsname == BASIN_HOP_TAG) {
        int nhop;
        double ampl;
        double eps;
        Basinhop<double>* bh;
        LocalOptimizer<double>* ls;
        std::map<std::string, std::string> tls;
        pars.parse(lsdata, tls);                    
        if(tls.find(BASIN_HOP_NHOP_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Number of tries not set");
        } else {
          nhop = atoi(tls[BASIN_HOP_NHOP_TAG].c_str());
        }
        if(tls.find(BASIN_HOP_AMPL_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Amplitude not set");
        } else {
          ampl = atof(tls[BASIN_HOP_AMPL_TAG].c_str());
        }
        if(tls.find(BASIN_HOP_LOCSEARCH_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Local search not set");
        } else {
          ls = LocalOptimizerFactory::create(tls[BASIN_HOP_LOCSEARCH_TAG], obj);
        }        
        
        int n = obj->getDim();
        
        bh = new Basinhop<double>();
	UniformCubic<double>* uc = new UniformCubic<double>();
	bh->setRandomStep(uc);
	bh->setInitStepLength(ampl);
	BoltzmannAcceptProbabilityFunction<double>* pf = new BoltzmannAcceptProbabilityFunction<double>();
	//pf->csetParam(ampl);
	bh->setAcceptProbabilityFunction(pf);
	ConstSchedule<double>* cs = new ConstSchedule<double>();
	bh->setTemperatureSchedule(cs);
	bh->setRangeSchedule(cs);        
        bh->setNumberOfStarts(nhop);
        bh->setLocalOptimizer(ls);
        locs = bh;
      } else if(lsname == LBFGS_TAG){
        double eps;
        std::map<std::string, std::string> tls;
        pars.parse(lsdata, tls);                    
        if(tls.find(CONJGRAD_EPS_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Precision not set");
        } else{
          eps = atof(tls[CONJGRAD_EPS_TAG].c_str());
        }
        MBFGS* lms = new MBFGS();
        lms->setObjective(obj);
        lms->setEps(eps);
        locs = lms;
      } else {
        BNB_ERROR_REPORT("Unsupported local optimizer");
      }
      return locs;
    }
};

#endif
