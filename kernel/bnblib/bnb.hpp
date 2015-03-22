#ifndef __BNB__H__
#define __BNB__H__

#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>

#include <string>
#include <queue>
#include <sstream>
#include <util/common/infini.hpp>

/**
 * @file bnb.hpp
 * Contains base classes and definitions for Branch-and-bound solver.
 *
 * @author \b Mikhail \b Posypkin 
 * The author can be reached at posypkin@ispras.ru
 *
 * @warning Using this library requires an explicit permition from author.
 */


/**
 * Problem types
 */

/**
 * Problem f(x) -> max
 */
#define BNB_MAXIMIZE 1

/**
 * Problem f(x) -> min
 */
#define BNB_MINIMIZE 0





/**
 * Generic class for problem data
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
class BNBProblemData
{
public:
  virtual void f() {}
};


/**
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 *
 * Generic solver class
 */
class BNBSolver {
public:

  /**
   * Set solver configuration
   */
  virtual void configure(std::string config) = 0;

  /**
   * Main solve process
   *
   */
  virtual void solve() = 0;

};


template <class SetFactory> class BNBTraverse {
public:

  virtual const typename SetFactory::Solution & getBestSolution() const = 0;
  
};

 /**
  * Informational structure for debugging and control
  */
  struct BNBBranchInfo 
  {

    /**
     * Constructor
     */
    BNBBranchInfo()
    {
      refresh();
    }

    /**
     * Reset structure fields
     */
    void refresh()
    {
      mBranched = 0;
      mSolutioned = 0;
      mDiscardedByRecord = 0;
      mDiscardedByConstraints = 0;
      mMaxCandidateProblems = 0;
      mEleminated = 0;
    }

    /**
     * Serialize structure to string
     *
     * @return composed string
     *
     */
    std::string toString()
    {
      std::ostringstream os;

      os << "Branched problems: " << mBranched.toString() << "\n"
          << "Problems became solutions: " << mSolutioned.toString() << "\n"
          << "Problems discarded by record(incumbent value): " << mDiscardedByRecord.toString() << "\n"
          << "Problems discarded by constraints: " << mDiscardedByConstraints.toString() << "\n"
          << "Problems eleminated by reduction procedures: " << mEleminated.toString() << "\n"
          << "Maximal number of candidate problems: " << mMaxCandidateProblems.toString() << "\n"
	 << "Total number of vertices in the search tree: " << (mBranched + mSolutioned + mDiscardedByRecord + mDiscardedByConstraints + mEleminated).toString() << "\n";

      return os.str();	
    }
    
    /**
     * Return total number of vertices in the search tree
     *
     * @return number of vertices in the search tree
     *
     */
    InfiniInt getTotalVerts()
    {
      return mBranched + mSolutioned + mDiscardedByRecord + mDiscardedByConstraints + mEleminated;
    }
    /**
     * Number of branched subproblems
     */
    InfiniInt mBranched;

    /**
     * Number of subproblems which became solutions
     */
    InfiniInt mSolutioned;

    /**
     * Subproblems discarded because the record is better
     */
    InfiniInt mDiscardedByRecord;

    /**
     * Subproblems discarded because of constraint violations
     */
    InfiniInt mDiscardedByConstraints;

    /**
     * Maximal number of candidate problems
     */
    InfiniInt mMaxCandidateProblems;
    
    /**
     * Problems eleminated by reduction procedure
     */
    InfiniInt mEleminated;
  };


#endif
