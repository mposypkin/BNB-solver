#ifndef __CHAINSOLGEN__HPP__
#define __CHAINSOLGEN__HPP__

#include <vector>
#include "asolgenerator.hpp"

template <class ProblemFactory> class ChainSolutionGenerator : 
           public AbstractSolutionGenerator <ProblemFactory> {
  public:
    
    typedef typename ProblemFactory::Solution Solution;
    typedef  AbstractSolutionGenerator < ProblemFactory > ASG;
    
    /**
     * Generate and pack to vector n feasible solutions
     * @param n 
     * @param solv 
     */
    void generate(int n, GVector < Solution > &solv)
    {
      if(!mSolgen.empty()) {
        typename std::vector< ASG* >::iterator i = mSolgen.begin();      
        int m = n;        
        while((i != mSolgen.end()) && (m > 0)) {        
          printf("BEFORE m = %d\n", m);
          (*i)->generate(m, solv);
          m = n - solv.size();
          printf("AFTER m = %d\n", m);
          i ++;
        }
      }
    }
    
    /**
     * Adds a solution generator
     * @param asg abstrac solution generator
     */
    void add(ASG* asg)
    {
      mSolgen.push_back(asg);
    }
    
   private:
     std::vector< ASG* > mSolgen;               
};

#endif
