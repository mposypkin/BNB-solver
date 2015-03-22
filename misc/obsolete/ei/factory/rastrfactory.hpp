#ifndef __RASTRFACTORY_HPP__
#define __RASTRFACTORY_HPP__

#include <util/xml/xmlpars.hpp>
#include <problems/cover/uncons/rastr.hpp>

#define RASTR_DIM_TAG "dim"




template <class FT> class RastrFactory {
  public:

    /**
      * Create objective for Rastrigin
      * @param odata objective data
      * @return objective
      */
    static Objective<FT>* create(const std::string& odata)
    {
      XMLPars pars;
      std::map<std::string, std::string> tls;
      Potential<FT>* pot;
      int natoms, dim;

      pars.parse(odata, tls);
      if(tls.find(RASTR_DIM_TAG) == tls.end()){
	BNB_ERROR_REPORT("Space dimension not set");
      } else {
	dim = atoi(tls[MOLEC_DIM_TAG].c_str());
      }
      Objective<FT>* obj = new Rastr<FT> (dim, 10.);
      return obj;
    }
};

#endif
