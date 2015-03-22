#ifndef __BNB_SOLUTION_SURFACE_HPP__
#define __BNB_SOLUTION_SURFACE_HPP__

/**
 * Abstaract class for holding solution surface
 */

template <class ProblemFactory> class BNBSolutionSurface {
  public:
    typedef typename ProblemFactory::Solution Solution;
    
    /**
     * Put solution
     * @param s solution to put
     */
    virtual void put(Solution& s)  = 0;
};

#endif
