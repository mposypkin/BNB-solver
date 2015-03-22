#ifndef __SIMPLSUF__
#define __SIMPLSUF__

#include <sstream>
#include <util/common/bnberrcheck.hpp>
#include "asolupdfilter.hpp"
#include "singlesolucont.hpp"
#include <util/common/util.hpp>
#include <util/common/random.hpp>
 
/**
 * The simplistic filter for solution based on a treshold.
 */
template < class ProblemFactory > class SimpleSolutionUpdateFilter : 
       public  AbstractSolutionUpdateFilter < ProblemFactory >
{

public:

  typedef AbstractSolutionUpdateFilter < ProblemFactory > Base;
  typedef typename ProblemFactory::Solution Solution;

  SimpleSolutionUpdateFilter()
  {
    mProb = 0.;
  }

  
  /**
   * Setup accept probability for bad solutions
   * @param  prob probability
   */
  void setAcceptProbability(double prob) {
    mProb = prob;
  }
  
  /**
   * Checks whether there is a time to activate of solution
   * transfer.
   *
   * @param solutions the bunch of solutions
   */
  bool activate()
  {   
    return !(mSendContainer.empty());
  }

  /**
   * Set up a package to send to a slave
   *
   * @param solutions the bunch of solutions
   * @param solv the target package (vector)
   */
  void setupSolutionPackage(std::vector < Solution > &solv)
  {
    while(!(mSendContainer.empty())) {
      Solution sol;
      sol = mSendContainer.back();
      mSendContainer.pop_back();
      solv.push_back(sol);
    }
  }

  std::string toString()
  {
    BNB_ERROR_REPORT("NOT IMPLEMENTED");
  }
  


  inline void push(const Solution & s)
  {      
    bool put = false;
    if(mSSC.empty())
      put = true;
    else if(ProblemFactory::getProblemType() == BNB_MAXIMIZE) {
      if(s.getValue() > mSSC.top().getValue())
        put = true;
    } else {
      if(s.getValue() < mSSC.top().getValue())
        put = true;
    }
    put = put || Coin::throwCoin(mProb);
    if(put)
      mSendContainer.push_back(s);
    mSSC.push(s);
  }

  inline bool empty() const
  {
    return mSSC.empty();
  }

  inline const Solution& top() const
  {
    return mSSC.top();
  }


private:
  SingleSolutionContainer < ProblemFactory > mSSC;  
  std::vector < Solution > mSendContainer;
  double mProb;
};
 

#endif
