#ifndef __SINGLESOLUCONT_HPP__
#define __SINGLESOLUCONT_HPP__

#include "solutioncont.hpp"

/**
 * The class for storing just one (best) solution found so far.
 * Set is a class for search tree vertice (set)
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
template <class ProblemFactory> class SingleSolutionContainer : public SolutionContainer < ProblemFactory>
{
  public:

    typedef typename SolutionContainer<ProblemFactory>::Solution Solution;

   /**
    * Constructor
    *
    */
    SingleSolutionContainer()
    {
      mHasSolution = false;
    }


   /**
    * Remove the solution
    */
    inline void pop()
    {
      mHasSolution = false;
    }

    /**
     * Store a solution
     *
     * @param s solution to store
     *
     */
    inline void push(const Solution & s)
    {      
      if(ProblemFactory::getProblemType() == BNB_MAXIMIZE) {
        if(s.getValue() > mSolution.getValue()){
          mSolution = s;
          mHasSolution = true;
        }
      } else {
        if(s.getValue() < mSolution.getValue()){
          mSolution = s;
          mHasSolution = true;
        }
      }
    }

    /**
     * Remove solutio from the container
     *
     */
    inline void clear() 
    {
      mHasSolution = false;
    }


    /**
     * Checks whether the container is actually empty
     *
     * @return true if the container is empty, false otherwise
     *
     */
    inline bool empty() const
    {
      return !mHasSolution;
    }

    /**
     * Retrieve best solution (for this case the only one :)
     *
     * @param s where to put the solution
     *
     */
    inline const Solution& top() const
    {
      return mSolution;
    }


    /**
     * Size of the solution container
     *
     * @return 1 or zero (solution exists or not)
     *
     */
    inline int size() const
    {
      if(mHasSolution)
        return 1;
      else 
        return 0;
    }

  private:

  /**
   * true is solution is already found
   */
    bool mHasSolution;

  /**
   * Best solution is stored here
   */
    Solution mSolution;
};
#endif
