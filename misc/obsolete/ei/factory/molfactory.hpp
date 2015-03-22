#ifndef __MOLFACTORY_HPP__
#define __MOLFACTORY_HPP__

#include <problems/molec/common/intmol.hpp>
#include <problems/molec/common/mors.hpp>
#include <problems/molec/common/lenjon.hpp>
#include <problems/molec/common/dzugutov.hpp>
#include <util/xml/xmlpars.hpp>

#define MOLEC_DIM_TAG "dim"

#define MOLEC_NATOMS_TAG "natoms"

#define MOLEC_POTENTIAL_TAG "potent"

#define POTENTIAL_MORS_TAG "mors"

#define POTENTIAL_LJ_TAG "lj"

#define POTENTIAL_DZUG_TAG "dzug"




template <class FT> class MolFactory {
  public:

    /**
      * Create objective for molecular conformation problem
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
      if(tls.find(MOLEC_DIM_TAG) == tls.end()){
	BNB_ERROR_REPORT("Space dimension not set");
      } else {
	dim = atoi(tls[MOLEC_DIM_TAG].c_str());
      }
      if(tls.find(MOLEC_NATOMS_TAG) == tls.end()){
	BNB_ERROR_REPORT("Number of atoms not set");
      } else {
	natoms = atoi(tls[MOLEC_NATOMS_TAG].c_str());
      }
      if(tls.find(MOLEC_POTENTIAL_TAG) == tls.end()){
	BNB_ERROR_REPORT("Potential not set");
      } else {
	std::map<std::string, std::string> tls1;
	pars.parse(tls[MOLEC_POTENTIAL_TAG], tls1);
	if(tls1.begin()->first == POTENTIAL_MORS_TAG) {
	  FT rho;
	  rho = atof(tls1.begin()->second.c_str());
	  pot = new Mors<FT> (rho);
	} else if(tls1.begin()->first == POTENTIAL_LJ_TAG) {
	  pot = new LenJon<FT>();
	} else if(tls1.begin()->first == POTENTIAL_DZUG_TAG) {
	  pot = new Dzugutov<FT>();
	} else {
	  BNB_ERROR_REPORT("Potential type not supported");
	}
      }
      Objective<FT>* obj = new IntervalMolec<FT> (natoms, dim, pot);
      return obj;
    }
};

#endif
