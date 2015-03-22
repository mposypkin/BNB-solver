#ifndef _BNBDUMMY_HPP_
#define _BNBDUMMY_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbdummy.hpp
 *
 * Dummy problem and solver for debugging and educational 
 * purposes.
 */

#include <stdlib.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include "bnb.hpp"


class BNBDummyProblemData : public BNBProblemData
{
public:

/**
 * Fills from string
 *
 * @param str string to fill from
 *
 */
  void fromString(std::string & str)
  {
    mTimeOut = atoi(str.c_str());
  }

/**
 * Writes for string
 *
 * @param str string to write to
 *
 */
  void toString(std::string & str)
  {
    std::ostringstream os;

    os << mTimeOut;
    str = os.str();
  }

/**
 * Accessor for TimeOut
 *
 * @return time out value
 *
 */
  int getTimeout()
  {
    return mTimeOut;
  }

private:
  int mTimeOut;
};

class BNBDummySolver : public BNBSolver 
{
public:
/**
 * Constructor
 *
 * @param cfg solver configuration
 *
 */
  BNBDummySolver(BNBProblemData * pd)
  {
    BNBDummyProblemData *dpd;
    
    dpd = dynamic_cast <BNBDummyProblemData*> (pd);    
    mTimeOut = dpd->getTimeout();

    printf("BNBDummySolver created\n");
    fflush(stdout);
  }

  /**
   * Configure
   */
   void configure(std::string config)
   {
   }
/**
 * "Solving" the problem
 *
 */
  void solve()
  {
    sleep(mTimeOut);
  }

private:
  int mTimeOut;

};


#endif
