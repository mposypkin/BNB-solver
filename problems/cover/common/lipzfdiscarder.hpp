#ifndef __LIPZFDISCARDER_HPP__
#define __LIPZFDISCARDER_HPP__

#include <math.h>
#include "go.hpp"
#include "lipzobjective.hpp"
#include <util/common/gvector.hpp>
/**
 * A discarder based on a Lipzic formula for a function
 */
template <typename FT> class LipzFuncDiscarder : public GOFactory<FT>::Discarder 
{
  
  public:
    
    struct Options {
      const static unsigned int NO_OPTION = 0;
      /**
	* Perform record update (normally turn off for constraints)
	*/
      const static unsigned int UPDATE_RECORD = 1;
    };
    
    LipzFuncDiscarder()
    {
      mObj = NULL;
      mOptions = Options::NO_OPTION;
      mOptions |= Options::UPDATE_RECORD;
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
    void setObjective(LipzObjective<FT>* obj)
    {
      mObj = obj;
    }
    
    /**
     * Setup precision
     * @param eps epsilon
     */
    void setEps(FT eps)
    {
      mEps = eps;
    }
    
    void discard( typename GOFactory<FT>::Set& s, GVector< typename GOFactory<FT>::Set >& sv, GVector< typename GOFactory<FT>::Solution>& solv, typename GOFactory<FT>::ValueType record, BNBBranchInfo* binfo)
    {
      bool rv;
      FT l, v, r;
      FT C[MAX_NUMBER_OF_VARS];
      int n = mObj->getDim();
      int imax = 0;
      FT rimax = 0;
      BNB_ASSERT(mObj);
      l = getLipz(s);      
//      printf("%s, %s, [%lf]\n", ((GOFactory<double>::Discarder*)this)->getName().c_str(), s.toString().c_str(), l);
      r = 0;
      for(int i = 0; i < n; i ++) {
	FT ri;
        C[i] = 0.5 * (s.mA[i] + s.mB[i]);        
	ri = s.mB[i] - C[i];
        r += BNBSQR(ri);
	if(ri > rimax) {
	  rimax = ri;
	  imax = i;
	} 

	//printf("%d: (%lf, %lf) ri = %lf, rimax = %lf\n", i, s.mA[i], s.mB[i],ri, rimax);
      }
      r = sqrt(r);
      v = mObj->func(C);
      if(mObj->getKind() == Objective<FT>::EQCONSTRAINT) {
        FT estim = BNBABS(v) - l * r;
        rv = (estim > 0);
      } else if(mObj->getKind() == Objective<FT>::INEQCONSTRAINT) {
        FT estim = v - l * r;
        rv = (estim > 0);
      } else {
        FT estim = v - l * r;
        s.mEstim = estim;
	if((Options::UPDATE_RECORD & mOptions) && (v < record)) {
	  typename GOFactory<FT>::Solution sol(n);
	  for(int i = 0; i < n; i ++) {
	    sol.mX[i] = C[i];
	  }
	  sol.mFval = v;
	  solv.push_back(sol);
	  record = v;
	}

        rv = (s.mEstim >= record - mEps);
	
	/*
	printf("estim = %lf, record = %lf, fv = %lf, eps = %lf\n", estim, record, v, mEps);
	printf("rimax = %lf, rbox = %lf, rboxi = %lf, re = %lf \n", rimax, r, sqrt(r * r - rimax * rimax), (v - record + mEps) / l);
	printf("rv = %s\n", rv ? "true" : "false");
	*/
	
      }
      if(rv == false) {
	FT R;
	if((mObj->getKind() == Objective<FT>::EQCONSTRAINT)) {
	  R = BNBABS(v) / l;
	} else if(mObj->getKind() == Objective<FT>::INEQCONSTRAINT) {
	  R = v / l;
	} else {
	  R = (v - record + mEps) / l;
	}
	s.mDiscardRho = BNBMAX(s.mDiscardRho, R);
	Segment<FT> seg;
//	printf("box to process: %s, r = %lf, R = %lf, l = %lf, record = %lf\n", BoxUtils::toString(s).c_str(), s.mDiscardRho, R, l, record);
	if(BoxUtils::cutL2(s.mSplitI, s, C, BNBSQR(s.mDiscardRho), seg)) {
	  s.mSegments.push(seg);
	}
	sv.push_back(s);
      } else {
        if((mObj->getKind() == Objective<FT>::EQCONSTRAINT) ||
	             (mObj->getKind() == Objective<FT>::INEQCONSTRAINT)) {
          binfo->mDiscardedByConstraints ++;
        } else {
          binfo->mDiscardedByRecord ++;
        }
      }
    }
    
  private:
    
    FT getLipz(const typename GOFactory<FT>::Set& s) const 
    {
      return mObj->getFuncLipzConstant(s.mA, s.mB);
    }
    
    FT mEps;
    LipzObjective<FT>* mObj;
    unsigned int mOptions;
};
#endif
