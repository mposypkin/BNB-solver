#ifndef __LOCOPTIM__
#define __LOCOPTIM__

#include "objective.hpp"

template <typename FT> class LocalOptimizer {
  public:
  /**
   * Perform search
   * @param x start point and result
   * @param v 
   * @return true if search converged and false otherwise
   */
    virtual bool search(FT* x, FT* v) = 0;
    
    /**
     * Setup objective
     * @param obj objective
     *
     */
    virtual void setObjective(Objective<FT>* obj)
    {
        mObj = obj;
    }
    
    /**
     * Retrieve objective
     * @return objective
     */
    virtual Objective<FT>* getObjective()
    {
        return mObj;
    }

    /**
     * Retrieve the number of steps (function calls)
     * @return number of steps
     */
    virtual int getSteps() {
        return 0;
    }

    /**
     * Resets number of steps to zero
     */
    virtual void resetSteps() {

    }
protected:
    Objective<FT> *mObj;
};
#endif
