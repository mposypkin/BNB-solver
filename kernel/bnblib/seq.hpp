#ifndef _SEQ_HPP_
#define _SEQ_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file seq.hpp
 *
 * Sequential bnb solver
 */


#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <sstream>

#include "util/common/util.hpp"
#include "bnb.hpp"
#include "util/common/bnblog.hpp"
#include "wtraverse.hpp"
#include "util/common/stdvector.hpp"

/**
 * Main class for BNB solver.
 *
 * This class is aimed at branch-and-bound solving integer programming
 * problems. The solution is found by iterative refining of problem
 * space.  
 * 
 * The class maintains two ordered queues: task queue and solution queue. 
 * Solving starts from task queue consisting of the initial set. On
 * each step top set from the task queue is removed and divided 
 * into a number of subsets. The process is finished when all leave 
 * subsets contains one element. 
 *
 * The subset is discarded if the best objective value found by the 
 * moment is better then the best estimation for this subset, i.e. 
 * the top of the solution queue is better then this subset.  
 * 
 * This class has two template parameters:
 *   Set - class for main set data
 *   SetFactory - class for creating instances of tSet
 *    
 * \b Set is supposed to provide the following interface:
 *
 *   \b bool \b isFinal()  - returns true if the set contains only 
 *                one element and false otherwise
 *
 *   \b tValue \b getValue() - returns best possible estimation for 
 *                elements of the set (in the case the 
 *                set if final it is the value of the 
 *                objective function) 
 *
 *   \b void \b setRecord(tValue record) - sets the value of the record (best
 *                objectivevalue found by the moment).  
 *
 *
 * \b SetFactory is supposed to provide the following interface:  
 *
 *   \b bool \b getInitialSet(Set& set) - fills the starting set to start search
 *                      (usually the search space), returns false if the search
 *                      space id empty
 *
 *   \b void \b branch(tSet set, std::vector < tSet > &sv) - derives 
 *                      vector sv of descendants from the set given 
 *                      as set
 *
 *   @attention It is supposed that Set is copied as a whole, i.e. if it contains pointer to data 
 *    objects then these data objects are copied with the set. This approach can be a bit expensive 
 *    because BNBSolver extensively copies sets from one data structure to another. Alternative
 *    feasible approach is provided by smart pointers with reference counting: when copiing only 
 *    the pointer to a data structure is copied and the reference counter is increased, the memory
 *    is released when the counter drops to zero.
 *
 * @author \b Mikhail \b Posypkin 
 * The author can be reached at posypkin@ispras.ru
 *
 * @warning Using this library requires an explicit permition from author. 
 */
template <class Traverse > class BNBSeqSolver : public
Traverse, public BNBSolver
{
public:
  typedef typename Traverse::Factory SetFactory;
  typedef typename SetFactory::Set Set;
  typedef typename SetFactory::Solution Solution;

  /**
   * The constructor.
   *
   * @param space computing space descriptor
   * @param fact set factory
   * @param solutionQueueTreshold maximal number of solution 
   * @param doLogging true if perform logging
   */
  BNBSeqSolver (SetFactory * fact) : Traverse (fact)
  {
    mMVT = -1;
    mMST = -1;
    mInfo.refresh();
    mDebug = Options::NO_DEBUG;
  }

  /**
   * Setup debug options
   * @param options 
   */
  void setDebugOptions(unsigned int options)
  {
    mDebug = options;
  }

  /**
    * Return debug options
    * @return reference to the debug options
    */
  unsigned int & getDebugOptions()
  {
    return mDebug;
  }

  /**
   * Configure 
   */
  void configure(std::string config)
  {
  }

/**
 * The solve process
 *
 * @param MVT maximal number of the vertices in the search tree
 *
 * @return number of vertices in the search tree
 *
 */
  void solve ()
  {
    Timer toverall ("overall time");
    InfiniInt n, c;
    StdVector < Set > sv;
    Set s;
    Set ss;
    Solution sol;
    c = 0;
    toverall.start ();

    if (Traverse::mSetFactory->getInitialSet (s)) {
      sv.push_back (s);
      Traverse::pushSets (sv, &mInfo);
    }
    

    if (Traverse::mSetFactory->getInitialSolution (sol))
      {
        StdVector < Solution > solv;
	if(mDebug & Options::PRINT_INITIAL_VAL) {
	  mLogger << "Initial Solution = " << sol.toString() << "\n";
	  mLogger.write();
	}
	solv.push_back (sol);
        Traverse::pushSolutions(solv);
      }
    

      if (mDebug & Options::PRINT_STATE)
      {
        mLogger << Traverse::getStateDescription();
        mLogger.write();
      }

    while (1)
      {
	n = Traverse::branch (&mInfo);
        if (mDebug & Options::PRINT_STATE)
	  {
	    mLogger << "\n\n--- STEP " << (++ c).toString() << " ---\n";
	    mLogger << Traverse::getStateDescription();
	    mLogger.write();
	  }
	if (n == (InfiniInt)0) 
	  break;
	else if(mInfo.getTotalVerts() >= mMST)
	  break;	
	else if(n >= mMVT)
	  break;
      }

    toverall.stop ();
    if(mDebug & Options::PRINT_RESULTING_STAT) {
      mLogger << "\n\n[sequential]\n" << toverall.toString () << "\n"
	 << mInfo.toString () << "\n";
      mLogger.write();
    }
  }

/**
 * Sets Maximal Vartices  in a queue Treshold. This corresponds to the number 
 * of pending candidate problems. 
 *
 * @param mvt MVT value
 *
 */
  void setMVT(InfiniInt mvt)
  {
    mMVT = mvt;
  }
  
  /**
    * Set Maximal Steps Treshold. Stop after this value is reached.
    * @param mst MST  value
    */
  void setMST(InfiniInt mst)
  {
    mMST = mst;
  }


/**
 * Retrieves staticstical information about performed operations
 * along the search.
 *
 * @return information structure reference
 *
 */
  BNBBranchInfo& getInfo()
  {
    return mInfo;
  }

 /**
  * Options to perform debugging
  */
 struct Options {
   static const int NO_DEBUG = 0;
   static const int PRINT_RESULTING_STAT = (1 << 1);
   static const int PRINT_STATE = (1 << 2);
   static const int PRINT_INITIAL_VAL = (1 << 3);
 };
  
private:
  
/**
 * Maximal number of vertices in the search tree
 */
  InfiniInt mMVT;

/**
 * Maximal number of terminal pending vertices
 */
  InfiniInt mMST;

  /**
   * Debug options
   */
  unsigned int  mDebug;
  
/**
 * Staticstical information about search operations performed along 
 * the solve process
 */
  BNBBranchInfo mInfo;
  
 /**
  * The logger
  */
  BNBLog<BNB_DO_LOGGING> mLogger;

};

#endif
