#ifndef _HEUR_HPP_
#define _HEUR_HPP_
/**
 * Base class for Heuristic search.
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file heur.hpp
 */

/**
 * Process kinds
 */

/**
 * Master process for heuristic search
 */
#define BNB_HEUR_MASTER 0

/**
 * Slave process for heuristic search
 */
#define BNB_HEUR_SLAVE 1

/**
 * Basic class for heuristic search.
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
template <class SetFactory, class SolutionContainer = std::priority_queue < typename SetFactory::Solution > > class BNBHeur {

public:
/**
 * The constructor
 *
 * @param fact set factory
 *
 */
  BNBHeur(SetFactory * fact)
  {
    typedef typename SetFactory::Solution Solution;
    mFactory = fact;
    mRecord = SetFactory::absoluteBound();
    mIterCounter = 0;
  }

/**
 * Retriev record value
 *
 * @return record value
 *
 */
  typename SetFactory::ValueType getIncumbentValue()
  {
    return mRecord;
  }


/**
 * Retrieve the value of the ieration counter
 *
 * @return 
 *
 */
  int getIterCounter()
  {
    return mIterCounter;
  }


/**
 * Assign the value of the iteration counter
 *
 * @param value new value of the iteration counter
 *
 */
  void setIterCounter(int value)
  {
    mIterCounter = value;
  }

protected:
/**
 * Set up heuristic search
 *
 * @param kind process kind 
 *
 * @param rank process number ( - 1 if the number is not important)
 *
 * @param size total number of working processes (- 1  if the number is not important)
 *
 */
  virtual void setup(int kind, int rank = -1, int size = -1)
  {
    mKind = kind;
    mRank = rank;
    mSize = size;
  }


/**
 * Set current heuristic
 *
 * @param heur
 *
 */
  virtual void setHeur(std::string heur)
  {
    mHeur = heur;
  }

/**
 * Perform one heuristic step
 *
 * @return true if step improved the record
 */
  virtual bool step()
  {
    return false;
  }


/**
 * Put solutions to the solution queue
 *
 * @param sv solutions
 *
 */
  virtual void pushSolutions(std::vector<typename SetFactory::Solution> & sv)
  {
    while(!sv.empty()) {
      typename SetFactory::Solution s;

      s = sv.back();
      sv.pop_back();
      if((SetFactory::getProblemType() == BNB_MAXIMIZE) ? s.getValue() >= mRecord : s.getValue() <= mRecord) {
	mRecord = s.getValue();
      }
      mSolutions.push(s);
    }
  }


  

/**
 * Solutions reside here
 */
  SolutionContainer mSolutions;
  
/**
 * Current heuristic
 */
  std::string mHeur;

/**
 * Set factory
 */
  SetFactory* mFactory;

/**
 * Record value
 */
  typename SetFactory::ValueType mRecord;

/**
 * Kind of the process
 */
  int mKind;

/**
 * My rank
 */
  int mRank;

/**
 * Total number of working slaves
 */
  int mSize;

  
/**
 * The counter of iterations
 */
  int mIterCounter;  
};

#endif
