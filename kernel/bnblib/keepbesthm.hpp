#ifndef __KEEPBESTHM_HPP__
#define __KEEPBESTHM_HPP__

#include <vector>
#include <deque>
#include "heurmaster.hpp"

/**
 * Generic class for controlling work distribution among
 * heuristic slaves.
 */
template <class ProblemFactory> class BNBKeepBestHeurMaster : public BNBHeurMaster <ProblemFactory> {
  public:
    
    typedef  BNBHeurMaster<ProblemFactory> BASE;
    typedef typename BNBHeurMaster<ProblemFactory>::Solution Solution;
    typedef typename ProblemFactory::ValueType VT;
    
    
    /**
     * Return string representation of the solution pool
     * @return string representation
     */
    std::string toString() const 
    {
      return "NOT IMPLEMENTED";
    }

    /**
     * Checks whether solution list is not empty
     * @return true if empty false otherwise
     */
    bool isSolutionListEmpty() const 
    {
      return mNotServed.empty();
    }
        
    
    /**
     * Put a set of solutions to a local pool
     * @param  vector of solutions
     * @param  stage of putting solutions
     * @return true if solutions improved the incumbent
     */
    bool pushSolutions(std::vector<Solution> &sv, typename BASE::PutStages stage)
    {
      bool rv = false;      
      typename std::vector<Solution>::const_iterator i;
      
      for(i = sv.begin(); i != sv.end(); i ++) {
        bool urv = BASE::updateBestSolution(*i);
        rv = rv || urv;
      }
      if(rv)
        renewNotServed();
      return rv;
    }
    
     /**
      * Retrieve solutions from a pool
      * @param n number of solutions
      * @param sv solution vector
      */
     void getSolutions(int n, std::vector<Solution> &sv)
     {
       if(!BASE::mHasBestSolution)
         sv.push_back(BASE::mBestSolution);
     }
  
  
    /**
     * Find best matching solutions to send to a slave from free slaves list.
     * Modify free slaves list accordingly. Form solutions vector and a destination
     * free slave number.
     * @param freeSlaves list of free slaves
     * @param sv vector of solutions
     * @param s pointer to the free slave to send
     * @return true if best match was found false otherwise
     */
     bool findBestMatch(std::deque<int> &freeSlaves , std::vector<Solution> &sv, int &s) {
       bool rv = false;
       std::deque<int>::iterator i;
       if(BASE::mHasBestSolution && (!mNotServed.empty())){
         for(i = freeSlaves.begin(); i != freeSlaves.end(); i ++) {
           std::vector<int>::iterator p;
           p = std::find(mNotServed.begin(), mNotServed.end(), *i); 
           if(p != mNotServed.end()) {
             rv = true;
             s = *p;
             freeSlaves.erase(i);
             mNotServed.erase(p);
             sv.push_back(BASE::mBestSolution);
             break;
           }
         }
       }
       return rv;
     }
    
  private:
    
    void renewNotServed()
    {
      mNotServed = *BASE::mSlaves;
    }
    
    std::vector< Solution > mSC;  
    std::vector< int > mNotServed;
};
#endif
