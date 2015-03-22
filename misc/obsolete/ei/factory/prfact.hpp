#ifndef _PRFACT_HPP_
#define _PRFACT_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file prfact.hpp
 *
 * Problem factory - create initial data for different problems.
 */

#include <string>
#include <kernel/bnblib/bnb.hpp>
#include <kernel/bnblib/bnbdummy.hpp>
#include <problems/knapsack/knapsack_bnb/common/knap.hpp>
#include <problems/cover/common/go.hpp>
#include <util/xml/xmlpars.hpp>
#include <problems/crypto/cryptodata.hpp>

#include "objfactory.hpp"
#include "boxfactory.hpp"
#include "cryptofactory.hpp"

#define BNB_GLOB_TAG "glob"

#define BNB_CRYPTO_TAG "crypto"

#define BNB_KNAP_TAG "knap"

class BNBProblemFactory
{
public:

/**
 * Creates problem data instance according to 
 * problem descriptor
 * @param problemdsc problem descriptor
 * @return problem data structure pointer
 */
   static BNBProblemData * createProblemData(const std::string& problemDsc)
  {   
    BNBProblemData* pd;
    XMLPars pars;    
    std::map<std::string, std::string> tls;
    pars.parse(problemDsc, tls);
    std::string pname, pdata;
    pname = tls.begin()->first;
    pdata = tls.begin()->second;
    if(pname == BNB_GLOB_TAG) {
      std::string objdata, boxdata;
      Objective<double>* obj;
      GOInitialData<double>* gid;
      tls.clear();
      pars.parse(pdata, tls);
      if(tls.find(BNB_OBJECTIVE_TAG) == tls.end()) {
        BNB_ERROR_REPORT("Objective not set");
      } else {
        objdata = tls[BNB_OBJECTIVE_TAG];
      }        
      if(tls.find(BNB_BOX_TAG) == tls.end()) {
        BNB_ERROR_REPORT("Box not set");
      } else {
        boxdata = tls[BNB_BOX_TAG];
      }        
      gid = new GOInitialData<double>();
      obj = ObjFactory<double>::create(objdata);
      gid->mObj = obj;
      BoxFactory::create(boxdata, gid);
      pd = gid;
    } else if(pname == BNB_CRYPTO_TAG) {
      std::string cnf;
      tls.clear();
      pars.parse(pdata, tls);
      if(tls.find(CRYPTO_CNF_TAG) == tls.end()) {
        BNB_ERROR_REPORT("CNF not found");
      } else {
        cnf = tls[CRYPTO_CNF_TAG];
      }        
      CryptoData * cd = new CryptoData();
      cd->fromString(cnf);
      pd = (BNBProblemData*)cd;
    } else
      BNB_ERROR_REPORT("Unsupported problem type"); 
    
    return pd;
  }

};



#endif
