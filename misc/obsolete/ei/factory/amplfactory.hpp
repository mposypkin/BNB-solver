#ifndef __AMPLFACTORY_HPP__
#define __AMPLFACTORY_HPP__

#include <util/xml/xmlpars.hpp>
#include <problems/ampl/common/amplobjective.hpp>

#define BNB_AMPL_STUB_TAG "ampl_stub"

#define STUB_TEXT_TAG "stub_text"

#define STUB_FILE_TAG "stub_file"

template <class FT> class AmplFactory {
  public:
    /**
      * Create objective from a problem described in AMPL and encoded
      *   as an AMPL stub
      * @param odata stub contents or a path to a stub (.nl) file
      * @return objective
      */
    static Objective<FT>* create(const std::string& odata)
    {
      XMLPars pars;
      std::map<std::string, std::string> tls;
      tls.clear();
      pars.parse(odata, tls);
      if (tls.find(STUB_TEXT_TAG) != tls.end()) {
        return new AmplObjective<FT>(tls[STUB_TEXT_TAG], false);
      } else if (tls.find(STUB_FILE_TAG) != tls.end()) {
        return new AmplObjective<FT>(tls[STUB_FILE_TAG], true);
      }
      BNB_ERROR_REPORT("AMPL stub data not set");
      return NULL;
    }
};

#endif // __AMPLFACTORY_HPP__

