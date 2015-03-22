#ifndef __LINCONSDISCARDER_HPP__
#define __LINCONSDISCARDER_HPP__

#include <math.h>
#include "go.hpp"
#include "linobjective.hpp"
#include <util/common/gvector.hpp>
/**
 * A discarder for linear constraints
 */
template <typename FT> class LinConsDiscarder : public GOFactory<FT>::Discarder 
{
  
  public:
    
    struct Options {
      const static unsigned int NO_OPTION = 0;
      const static unsigned int DO_CUT = 1;
    };
    
    LinConsDiscarder()
    {
      mObj = NULL;
      mOptions = Options::NO_OPTION;
    }
    
   /**
    * Retrieve options reference
    * @return options reference
    */
    unsigned int& getOptions()
    {
      return mOptions;
    }
        
    /**
     * Setup objective pointer
     * @param obj objective
     */
    void setObjective(LinObjective<FT>* obj)
    {
      mObj = obj;
    }
    
    void discard( typename GOFactory<FT>::Set& s, GVector< typename GOFactory<FT>::Set >& sv, GVector< typename GOFactory<FT>::Solution>& solv, typename GOFactory<FT>::ValueType record, BNBBranchInfo* binfo)
    {
      bool rv;
      FT U[MAX_NUMBER_OF_VARS];
      BNB_ASSERT(mObj);
      int n = mObj->getDim();
      FT* A = mObj->getCoeff();
      for(int i = 0; i < n; i ++) {
	if(A[i] >= 0.) {
	  U[i] = s.mA[i];        
	} else if(A[i] < 0.) {
	  U[i] = s.mB[i];
	}
      }
      FT v = mObj->func(U);
      /*
      for(int i = 0; i < n; i ++) {
	printf("%lf ", A[i]);
      }
      printf(": ");
      for(int i = 0; i < n; i ++) {
	printf("[%lf ", s.mA[i]);
	printf("%lf] ", s.mB[i]);
	printf("%lf ", U[i]);
      }
      VecUtils::vecPrint(n, (double*)s.mA);
      VecUtils::vecPrint(n, (double*)s.mB);
      printf("e = %lf\n", v);
      */
      if(v <= 0.) {
	rv = false;
      } else {
	rv = true;
      }
      if((!rv) && (mOptions & Options::DO_CUT)) {
	for(int i = 0; i < n; i ++) {
	  if(A[i] != 0) {
	    FT t = - v / A[i];
	    if(A[i] > 0) {
	      s.mB[i] = BNBMIN(s.mB[i], s.mA[i] + t);
	    } else {
	      s.mA[i] = BNBMAX(s.mA[i], s.mB[i] + t);
	    }
	  }
	}
      }
      if(rv == false) {
	sv.push_back(s);
      } else {
	binfo->mDiscardedByConstraints ++;
      }
    }
    
  private:
    
    LinObjective<FT>* mObj;
    unsigned int mOptions;
};
#endif
