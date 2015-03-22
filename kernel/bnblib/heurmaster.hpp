#ifndef __HEURMASTER_HPP__
#define __HEURMASTER_HPP__

#include <vector>
#include <deque>

/**
 * Generic class for controlling work distribution among
 * heuristic slaves.
 */
template <class ProblemFactory> class BNBHeurMaster {
  public:
    
    enum PutStages{
      FromMaster,
      FromBNB,
      FromHeur      
    };
    
    typedef typename ProblemFactory::Solution Solution;
    typedef typename ProblemFactory::ValueType VT;
    
    BNBHeurMaster()
    {
      mSlaves = NULL;
    }
    
    /**
     * Setup list of numbers of heuristic slaves
     * @param hs 
     */
    void setHeurSlaves(std::vector<int> &hs)
    {
      mHeurSlaves = hs;
      mSlaves = &mHeurSlaves;
      mHasBestSolution = false;
      mIncumbent = ProblemFactory::absoluteBound();
    }
        
    /**
     * Retrieve current incumbent
     * @return incumbent value 
     */
    VT getIncumbent() const
    {
      return mIncumbent;
    }
    
    /**
     * Updates new incumbent
     * @param incum incubent value
     * @return true if the new value is better than the old one
     */
    bool updateIncumbent(VT incum)
    {
      bool rv;
      if(ProblemFactory::getProblemType() == BNB_MAXIMIZE){
        if(incum > mIncumbent) {
          mIncumbent = incum;
          rv = true;
        } else
          rv = false;
      } else {
        if(incum < mIncumbent) {
          mIncumbent = incum;
          rv = true;
        } else
          rv = false;
      }
      return rv;
    }
    
    
    
    /**
     * Updates best solution found so far
     * @param sol best solution
     * @return true if solution was updated, false otherwise
     */
    bool updateBestSolution(const Solution& sol)
    {
      bool rv = false;
      if(((ProblemFactory::getProblemType() == BNB_MAXIMIZE) &&
           (sol.getValue() > mIncumbent)) || 
           ((ProblemFactory::getProblemType() == BNB_MINIMIZE) && 
           (sol.getValue() < mIncumbent))){
        mIncumbent = sol.getValue();
        mBestSolution = sol;
        mHasBestSolution = true;          
        rv = true;
           }
      return rv;
    }
    
    /**
     * Fills parameter vector with best solution
     * @param  solution vector
     * @return True if best solution exists
     */
    bool pickBestSolution(std::vector<Solution>& sv) const 
    {
      if(mHasBestSolution)
        sv.push_back(mBestSolution);
      return mHasBestSolution;
    }      
    
    
    /**
     * Return string representation of the solution pool
     * @return string representation
     */
    virtual std::string toString() const = 0;

    /**
     * Checks whether solution list is not empty
     * @return true if empty false otherwise
     */
    virtual bool isSolutionListEmpty() const = 0;
        
    
    /**
     * Put a set of solutions to a local pool
     * @param  vector of solutions
     * @param  stage of putting solutions
     * @return true if solutions improved the incumbent
     */
    virtual bool pushSolutions(std::vector<Solution> &sv, PutStages stage) = 0;
    
     /**
      * Retrieve solutions from a pool
      * @param n number of solutions
      * @param sv solution vector
      */
     virtual void getSolutions(int n, std::vector<Solution> &sv) = 0;
  
  
    /**
     * Find best matching solutions to send to a slave from free slaves list.
     * Modify free slaves list accordingly. Form solutions vector and a destination
     * free slave number.
     * @param freeSlaves list of free slaves
     * @param sv vector of solutions
     * @param s free slave to send
     * @return true if best match was found false otherwise
     */
    virtual bool findBestMatch(std::deque<int> &freeSlaves , std::vector<Solution> &sv, int &s) = 0;
    
    
  protected:
    /**
     * The value of incumbent
     */
    typename ProblemFactory::ValueType mIncumbent;
    /**
     * Best Solution
     */
    typename ProblemFactory::Solution mBestSolution;
    /**
     * True if best solution was found
     */
    bool mHasBestSolution;
    
    /**
     * Pointer to a list of all slaves
     */
    const std::vector<int>* mSlaves;
    
  private:
    
    /**
   * The list of heuristic slave numbers
     */
    std::vector<int> mHeurSlaves;
    
};
#endif
