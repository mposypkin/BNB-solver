#ifndef __LINESEARCH__
#define __LINESEARCH__

#include "objective.hpp"

/**
 * Generic class for implementing line search methods
 */
template <typename FT> class LineSearch {
  public:
    
    /**
     * Set up objective for minimization
     * @param obj objective
     */
    virtual void setObjective(Objective<FT>* obj) = 0;
    
    /**
     * Search for alpha
     * @param x initial point
     * @param dir (anti) search direction
     * @return best alpha
     */
    virtual FT search(FT* x, FT* dir) = 0;
};

#endif
