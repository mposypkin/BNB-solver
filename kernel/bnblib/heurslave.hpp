#ifndef __HEURSLAVE_HPP__
#define __HEURSLAVE_HPP__

/**
 * Generic interface for heuristic slave.
 */
template <class ProblemFactory> class BNBHeurSlave {
  public:
    typedef typename ProblemFactory::Solution Solution;
    typedef typename ProblemFactory::ValueType VT;
  
    BNBHeurSlave()
    {
      mFactory = NULL;
    }
    
    /**
     * Setup problem factory pointer
     */ 

     void setProblemFactory(ProblemFactory* fact)
     {
       mFactory = fact;
     }
     
    /**
     * Setup iteration counter
     * @param cnt iteration counter
     */
    void setIterCounter(int cnt)
    {
      mIterCounter = cnt;
    }
    
    /**
     * Retrieve iteration counter
     * @return iteration counter
     */
    int getIterCounter()
    {
      return mIterCounter;
    }
    
    /**
     * Setup the heuristic in accordance to process number 
     * @param p rank of concrete process
     * @param nofp number of processes for heuristic of this sort
     */
    virtual void setup(int p, int nofp) = 0;
    
    /**
     * Performs one heuristic step and updates the solution vector
     * @param s solution vector
     * @return true if the search is completed, false if the search is interrupted
     */
    virtual bool step(std::vector<Solution>& sv) = 0;
    
  protected:
    /**
     * The counter of successive iterations
     */
    int mIterCounter;
    /**
     * Pointer to the problem factory
     */
    ProblemFactory* mFactory;
    
};

#endif
