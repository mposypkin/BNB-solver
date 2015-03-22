#ifndef __STEPDISC_HPP__
#define __STEPDISC_HPP__

#include <math.h>
#include <map>
#include "go.hpp"

/**
  * Reduces box in the directions of dicrete variables
  */
template <class FT> class IntStepDiscarder : public GOFactory < FT >::Discarder{
  public:

    typedef typename GOFactory < FT >::Set Set;
    typedef typename GOFactory < double >::Solution Solution;

    /**
     * Default constructor (all variables are integral and all steps are ones)
     * @param n number of variables
     */
    IntStepDiscarder(int n) : GOFactory <FT>::Discarder() {
      for(int i = 0; i < n; i ++) {
	mSteps[i] = 1;
      }
    }

    /**
     * Constructor.
     * @param steps a map that contains steps for discrete variables
     */
    IntStepDiscarder(const std::map<int, FT>& steps) : GOFactory <FT>::Discarder() {
      mSteps = steps;
    }

    /**
      * Set steps
      * @param steps a map that contains steps for discrete variables
      */
    void setSteps(const std::map<int, FT>& steps) {
      mSteps = steps;
    }


    void discard(Set& s,
       	GVector< Set > &sv,
	GVector< Solution >& solv,
	double record,
	BNBBranchInfo* info)
    {
      bool rv;

      typename std::map<int, FT>::iterator it;
      for (it = mSteps.begin() ; it != mSteps.end(); it ++ ){
	rv = reduce(it->first, it->second, s);
	if(!rv) break;
      }
      if(rv)
	sv.push_back(s);
    }

  private:

    bool reduce(int i, FT h,Set& s)
    {
      int A = ceil (s.mA[i] / h);
      int B = floor (s.mB[i] / h);
      if(A > B) 
	return false;
      else {
	s.mA[i] = A * h;
	s.mB[i] = B * h;
	return true;
      }
    }

    std::map<int, FT> mSteps;
};

#endif
