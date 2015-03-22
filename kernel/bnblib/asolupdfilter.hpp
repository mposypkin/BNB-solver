#ifndef __ASOLUPDFILTER__
#define __ASOLUPDFILTER__
/**
 * A class for the abstract solution update filter for MSHeur class
 */

#include <string>
#include "solutioncont.hpp"

template < class ProblemFactory > class AbstractSolutionUpdateFilter : 
         public SolutionContainer <ProblemFactory> {

public:

  typedef typename SolutionContainer < ProblemFactory >::Solution Solution;
  /**
   * Checks whether there is a time to activate of solution
   * transfer.
   */
  virtual bool activate() = 0;

  /**
   * Set up a package to send to a slave
   * @param solv the target package (vector)
   */
  virtual void setupSolutionPackage(std::vector < Solution > &solv) = 0;
 
  
  /**
   * Serialize to string
   */
  virtual std::string toString() = 0;

};
 

#endif
