#ifndef __SOLUMAK_HPP__
#define __SOLUMAK_HPP__


#include <math.h>
#include <map>
#include "go.hpp"

/**
  * Class for making solutions for mixed-integer problems
  */
template <class FT> class IntSolumak : public GOFactory <FT>::SolutionMaker {
  public:

  typedef typename GOFactory <FT>::Set Set;
  typedef typename GOFactory <FT>::Solution Solution;

  /**
    * Default constructor (all variables are integral and all steps are ones)
    * @param n number of variables
    */
  IntSolumak(int n) : GOFactory <FT>::SolutionMaker()
  {
    for(int i = 0; i < n; i ++) {
      mSteps[i] = 1;
    }
  }

  /**
    * Constructor.
    * @param steps a map that contains steps for discrete variables
    */
  IntSolumak(const std::map<int, FT> steps) : GOFactory <FT>::SolutionMaker()
  {
    mSteps = steps;
  }

  /**
   * Set steps
   * @param steps a map that contains steps for discrete variables
   */
  void setSteps(const std::map<int, FT>& steps) {
    mSteps = steps;
  }

  bool make(const Set& s, Solution& sol)
  {
    int n = s.mDim;
    for(int i = 0; i < n; i ++) {
      sol.mX[i] = 0.5 * (s.mA[i] + s.mB[i]);
    }
    typename std::map<int, FT>::iterator it;
    for (it = mSteps.begin() ; it != mSteps.end(); it ++ ){
      correct(it->first, it->second, sol.mX, s.mA, s.mB);
    }
    return true;
  }

  private:

  void correct(int i, FT h, FT* v, FT* a, FT* b)
  {
    FT u = floor(v[i] / h) * h;
    FT U = ceil(v[i] / h) * h;
    if(u >= a[i])
      v[i] = u;
    else {
      if(U > b[i]) {
	printf("%lf, %lf\n", v[i]/h, ceil(12.0/*v[i] / h*/));
	printf("%d, a= %lf, b=%lf, v = %lf,  u = %lf, U = %lf\n", i, a[i], b[i], v[i], u, U);
	BNB_ERROR_REPORT("No feasible points in a box\n");
      }
      v[i] = U;
    }
  }

  std::map<int, FT> mSteps;

};
#endif
