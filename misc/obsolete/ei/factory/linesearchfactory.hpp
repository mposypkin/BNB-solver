#ifndef __LINESEARCH_HPP__
#define __LINESEARCH_HPP__

#include <problems/optlib/linesearch.hpp>
#include <util/xml/xmlpars.hpp>
#include <problems/optlib/fletchsearch.hpp>

/**
 * Line search types
 */

#define WOLF_LS_TAG  "wolf"

#define WOLF_EPS_TAG "eps"

#define WOLF_RE_TAG  "re"

#define WOLF_RHO_TAG "rho"

#define WOLF_SIGMA_TAG "sigma"

/**
 * Factory to produce line search classes
 */

class LineSearchFactory {
  public:
    
    static LineSearch<double>* createLineSearch(std::string descript)
    {
      XMLPars pars;
      LineSearch<double>* ls;
      printf("LINE SEARCH: [%s]\n", descript.c_str());
      std::map<std::string, std::string> tls;
      std::string lsname, lsdata;
      pars.parse(descript, tls);            
      lsname = tls.begin()->first;
      lsdata = tls.begin()->second;
      if(lsname == WOLF_LS_TAG) {
        std::map<std::string, std::string> tls;
        double eps, re, rho, sigma;
        pars.parse(lsdata, tls);
        if(tls.find(WOLF_EPS_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Epsilon not set\n");
        } else {
          eps = atof(tls[WOLF_EPS_TAG].c_str());
        }
        if(tls.find(WOLF_RE_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Right end not set\n");
        } else {
          re = atof(tls[WOLF_RE_TAG].c_str());
        }
        if(tls.find(WOLF_RHO_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Rho not set\n");
        } else {
          rho = atof(tls[WOLF_RHO_TAG].c_str());
        }
        if(tls.find(WOLF_SIGMA_TAG) == tls.end()) {
          BNB_ERROR_REPORT("Sigma not set\n");
        } else {
          sigma = atof(tls[WOLF_SIGMA_TAG].c_str());
        }

        FletchSearch<double> *lins;    
        lins = new FletchSearch<double>();        
        lins->setEps(eps);
        lins->setRightEnd(re);
        lins->setRho(rho);
        lins->setSigma(sigma);        
        ls = lins;
      } else {
        BNB_ERROR_REPORT("Line search type not supported");
      }
      return ls;
    }
};

#endif
