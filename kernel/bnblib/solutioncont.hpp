#ifndef __SOLUTION_CONTAINER_HPP__
#define __SOLUTION_CONTAINER_HPP__

/**
 * Generic for storing solutions of optimization problem
 */
template <class ProblemFactory> class SolutionContainer 
{
  public:
  
  typedef typename ProblemFactory::Solution Solution;
  typedef Solution value_type;
  
  /**
   * Remove the solution from top
   */
  //virtual void pop() = 0;  
  /**
   * Store a solution
   * @param s solution to store
   */
  virtual void push(const Solution & s) = 0;
  /**
   * Remove solutio from the container
   */
  //virtual void clear() = 0; 
  /**
   * Checks whether the container is actually empty
   * @return true if the container is empty, false otherwise
   */
  virtual bool empty() const = 0;

  /**
   * Retrieve best solution (for this case the only one :)
   */
  virtual const Solution& top() const = 0;
  
  /**
   * Size of the solution container
   * @return size
   */
  //virtual int size() const = 0;
  

};
#endif
