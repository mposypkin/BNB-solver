#ifndef __ASOLGENERATOR_HPP__
#define __ASOLGENERATOR_HPP__

#include <util/common/gvector.hpp>

template <class ProblemFactory> class AbstractSolutionGenerator {
  public:
    
    typedef typename ProblemFactory::Solution Solution;
    
    /**
     * Generate and pack to vector n feasible solutions
     * @param n 
     * @param solv 
     */
    virtual void generate(int n, GVector < Solution > &solv) = 0;
};

#endif
