#ifndef __NTRAVERSE_H__
#define __NTRAVERSE_H__
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file wtraverse.hpp
 */

#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <stack>

#include <util/common/bnblog.hpp>
#include <util/common/util.hpp>
#include "bnb.hpp"
#include "singlesolucont.hpp"
#include "util/common/stdvector.hpp"



/**
  * New class for testing with multicriteria optimization
  *
 */
template < class ProblemFactory, 
	   class TraverseQueue = std::stack <typename ProblemFactory::Set> > class NTraverse 
{
public:

  typedef ProblemFactory Factory;

  typedef typename ProblemFactory::Set Set;

  typedef typename ProblemFactory::Solution Solution;


 /**
  * Constructor
  *
  * @param fact set factory
  */
  NTraverse (ProblemFactory * fact)
  {
    mSetFactory = fact;
    mSetFactory->getInitialSolution(mIncumbentSolution);
    mDebug = 0;
  }


/**
 * Return the number of candidate problems.
 *
 * @return number of candidate problems
 *
 */
  TraverseQueue& getCandidateProblemsList()
  {
    return mTaskQueue;
  }


/**
 * Retrieve current incumbent solution
 *
 * @return incumbent solution
 *
 */
  typename ProblemFactory::Solution& getIncumbent()
  {
    return mIncumbentSolution;
  }

/**
 * Set incumbent solution
 *
 * @param sol incumbent solution 
 *
 */
  void setIncumbent(typename ProblemFactory::Solution& sol, BNBBranchInfo * info = NULL)
  {
    mIncumbentSolution = sol;
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
    * Options to perform debugging
    */
  struct Options {
    static const int NO_DEBUG = 0;
    static const int PRINT_STEP = (1 << 1);
    static const int PRINT_STATE = (1 << 2);
  };

protected:


  
  /**
   * Push sets into the set container 
   *
   * @param sv set vector
   * @param info informational parameter 
   */
  void pushSets (GVector < Set > &sv, BNBBranchInfo * info = NULL)
  {
    while (!sv.empty ()) {
      Set s;

      s = sv.back ();
      sv.pop_back ();
      //BNB_ASSERT(!mSetFactory->isFinal(s));
      if(!mSetFactory->discard (s, getIncumbent())) {
	mTaskQueue.push (s);
      } else if(info) {
	info->mDiscardedByRecord ++;
      }
    }
    
    if(info)  {
      InfiniInt tmpi;
      tmpi = mTaskQueue.size();
      info->mMaxCandidateProblems = BNBMAX(info->mMaxCandidateProblems, tmpi);
    }
  }

  /**
   * Push solutions into the solution container
   * 
   * @param sv solution vector
   * @param info informational parameter 
   */
  void pushSolutions (GVector < Solution > &sv, BNBBranchInfo * info = NULL)
  {
    while (!sv.empty ()) {
      Solution s;
      s = sv.back ();
      sv.pop_back ();
      
      mSetFactory->merge(getIncumbent(), s);
    }
  }

/**
 * Takes the best set from the task queue, compares it
 * with the best solution and if it appeares to be better
 * obtains a set of descendants from it. Otherwise the best
 * set is discarded. 
 *
 * @param hit true if branch operations was performed
 *
 * @return number of sets in the task queue
 *
 */
  int branch (BNBBranchInfo * info = NULL)
  {
    int n;

    n = mTaskQueue.size ();    
    if (n > 0){
	Set s = mTaskQueue.top ();
	mTaskQueue.pop ();
        if(mDebug & Options::PRINT_STEP) 
          mLogger << "\npoped: " << s.toString() << "\n";
        if (!mSetFactory->discard (s, getIncumbent())){
	    StdVector < Set > setv;
	    StdVector < Solution > solv;
            mSetFactory->branch (s, setv, solv, getIncumbent(), info, n);
	    pushSets (setv, info);
	    pushSolutions (solv, info);
	} else {
          if(mDebug & Options::PRINT_STEP) 
            mLogger << "discarded\n";
	  if(info)
	      ++ (info->mDiscardedByRecord) ;
	}
        if(mDebug & Options::PRINT_STEP) 
          mLogger.write();
    }
    
    n = mTaskQueue.size ();
    return n;
  }

/**
 * Produces state description
 *
 * @return string representation of the current state
 *
 */
  std::string getStateDescription()
  {
    std::ostringstream os;

    os << " *** INCUMBENT = " << getIncumbent().toString();
    os << "\n *** CANDIDATE PROBLEMS :\n" << toString (&mTaskQueue);

    return os.str();
  }

/**
 * Creates a string representation for a 
 * prioritized queue 
 * 
 * @param inQueue
 *
 */
  template <class Queue> std::string toString (Queue *inQueue)
  {
    typename Queue::value_type s;
    Queue dupq = *inQueue;
    std::ostringstream ostr;
    int sz;

    sz = dupq.size ();
    if(sz == 0 ) {
      ostr << "empty";
    } else {
      ostr << "size = " << sz ;
      ostr<< ", data: \n";
      while (!dupq.empty ())
	{
          s = dupq.top ();
	  ostr << s.toString () << "\n";
          dupq.pop ();
	}
    }
    return ostr.str ();
  }

  
/**
 *  The task queue
 */
  TraverseQueue mTaskQueue;


/**
 * The set factory
 */
  ProblemFactory *mSetFactory;


private:

  typename ProblemFactory::Solution mIncumbentSolution;

  BNBLog<BNB_DO_LOGGING> mLogger;

  unsigned int  mDebug;

};

#endif
