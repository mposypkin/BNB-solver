#ifndef __OBJFACTORY_HPP__
#define __OBJFACTORY_HPP__

#include <util/xml/xmlpars.hpp>
#include "objtags.hpp"
#include "molfactory.hpp"
#include "rastrfactory.hpp"
#include "amplfactory.hpp"

/**
 * Objective factory
 */
template <class FT> class ObjFactory {
  public:
    /**
      * Create objective by description
      * @param description objective description in XML formal
      * @return objective
      */
    static Objective<FT>* create(const std::string& description)
    {
      Objective<FT>* obj;
      XMLPars pars;
      std::map<std::string, std::string> tls;
      std::string oname, odata;
      pars.parse(description, tls);
      oname = tls.begin()->first;
      odata = tls.begin()->second;
      
      if(oname == BNB_MOLECULAR_TAG) {
        obj = MolFactory<FT>::create(odata);
      } else if(oname == BNB_RASTRIGIN_TAG) {
        obj = RastrFactory<FT>::create(odata);
      } else if(oname == BNB_AMPL_STUB_TAG) {
        obj = AmplFactory<FT>::create(odata);
      } else {
        BNB_ERROR_REPORT("Unsupported objective");
      }
      return obj;
    } 
};

#endif
