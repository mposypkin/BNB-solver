/* 
 * File:   sersolvfact.hpp
 * Author: posypkin
 *
 * Factory for serial solver
 *
 * Created on September 2, 2011, 12:11 PM
 */

#ifndef SERSOLVFACT_HPP
#define	SERSOLVFACT_HPP

#include <stdio.h>
#include <string>
#include <util/common/bnberrcheck.hpp>
#include <util/xml/xmlpars.hpp>

#include "solvertypes.hpp"
#include "serialsolver.hpp"
#include "slocfactory.hpp"

class BNBSerialSolverFactory
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
  static SerialSolver* createSolver(BNBProblemData* pd, const std::string& solverType)
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


};


#endif	/* SERSOLVFACT_HPP */

