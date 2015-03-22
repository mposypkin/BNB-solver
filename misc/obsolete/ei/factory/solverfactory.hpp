#ifndef _SOLVERFACTORY_HPP_
#define _SOLVERFACTORY_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file solverfactory.hpp
 *
 * Solver factory classes
 */

#include <stdio.h>
#include <string>
#include <kernel/bnblib/bnb.hpp>
#include <util/aclib/aclib.hpp>
#include "solvertypes.hpp"
#include <kernel/bnblib/bnbdummy.hpp>
#include <kernel/bnblib/clms.hpp>
#include <kernel/bnblib/wtraverse.hpp>
#include <util/common/bnberrcheck.hpp>
#include <kernel/bnblib/msheur2.hpp>
#include <problems/cover/common/go.hpp>
#include <problems/cover/common/gopack.hpp>
#include <kernel/bnblib/basicheurmaster.hpp>
#include <kernel/bnblib/keepbesthm.hpp>
#include <problems/cover/common/contheur.hpp>
#include <problems/optlib/basinhop.hpp>
#include <problems/cover/common/distdivers.hpp>
#include <util/xml/xmlpars.hpp>
#include "pmbhfactory.hpp"
#include "locoptimfactory.hpp"
#include <problems/optlib/boxpert.hpp>
#include <problems/crypto/cryptosolver.hpp>
#include <problems/cover/common/slocsolver.hpp>
#include "slocfactory.hpp"
#include "serialsolver.hpp"
#include "parmbhfactory.hpp"
#include "parknapsolver.hpp"
#include "multistartfactory.hpp"

#define MAX_RESOURCE_NAME_LENGTH 1024


class BNBSolverFactory
{
public:

/**
 * Create serial solver from problem data and solver type 
 *
 * @param pd pointer to the problem data
 *
 * @param solverType type of the solver
 *
 * @return created solver
 *
 */
  static SerialSolver* createSerialSolver(BNBProblemData* pd, const std::string& solverType)
  {
    SerialSolver* solver = NULL;
    std::string stype, sdata;
    std::map<std::string, std::string> mp;
    XMLPars pars;
    pars.parse(solverType, mp);                   
    stype = mp.begin()->first;
    sdata = mp.begin()->second;
    if(stype == BNB_SLOC_SOLVER) {
      solver = SLocSolverFactory::createSolver(pd, sdata);
    } else {
      BNB_ERROR_REPORT("Solver type not recognized\n");
    }
    return solver;
  }
  
/**
 * Create parallel (Distributed memory) solver from problem data, solver type and solver config
 *
 * @param pd pointer to the problem data
 *
 * @param gac pointer to the global abstract communicator with gate
 *
 * @parma ac pointer to abstract communicator
 *
 * @param solverType type of the solver
 *
 * @return created solver
 *
 */
  static BNBSolver* createDistMemSolver(BNBProblemData* pd, AbstractCommunicator* gac, AbstractCommunicator* ac, const std::string& solverType)
  {
    BNBSolver* s = NULL;
    std::string stype, sdata;
    std::map<std::string, std::string> mp;
    XMLPars pars;
    pars.parse(solverType, mp);                   
    stype = mp.begin()->first;
    sdata = mp.begin()->second;
    if(stype == BNB_PARALLEL_KNAP_SOLVER){
      s = ParKnapSolverFactory::createSolver(pd, gac, ac);
    } else if(stype == BNB_PARALLEL_MBH_SOLVER) {
      s = ParmbhFactory::createSolver(pd, gac, ac, sdata);
    } else if(stype == BNB_PARALLEL_MULTISTART_SOLVER) {
      s = MultistartFactory::createSolver(pd, gac, ac, sdata);
    } else if(stype == CRYPTO_SOLVER) {
      s = new CryptoSolver(gac, pd);
    } else if(stype == BNB_DUMMY_SOLVER) {
      s = new BNBDummySolver(pd);
    } else {
      printf("%s\n", stype.c_str());
      BNB_ERROR_REPORT("Solver type not supported\n");
    }
    return s;
  }
  
};

#endif
