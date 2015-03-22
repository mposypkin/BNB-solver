#ifndef __PARKNAPSOLVER_HPP__
#define __PARKNAPSOLVER_HPP__

#include <stdio.h>
#include <string>
#include <kernel/bnblib/bnb.hpp>
#include <util/aclib/aclib.hpp>
#include <kernel/bnblib/clms.hpp>
#include <kernel/bnblib/wtraverse.hpp>
#include <problems/knapsack/knapsack_bnb/parallel_dm/knappar.hpp>
#include <util/common/bnberrcheck.hpp>


/**
  * Create branch-and-bound knapsack solver
  * 
  * @param pd Problem Data
  * @param gac Global Abstract Communicator
  * @param ac Abstract Communicator
  * @return newly created BNBSolver
 */

class ParKnapSolverFactory {
  public:

    static BNBSolver* createSolver(BNBProblemData* pd, AbstractCommunicator* gac, AbstractCommunicator* ac)
    {
      tKSInitialData* id;
      tKSSetFactory* fact;
      BNBSolver* s;

      id = dynamic_cast<tKSInitialData*> (pd);
      fact = new tKSSetFactory(id);
      s = new BNBClmsSolver < WTraverse < tKSSetFactory> >   ( gac, ac, fact);
      return s;
    }
};

#endif

