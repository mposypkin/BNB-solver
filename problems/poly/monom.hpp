#ifndef __MONOM_HPP__
#define __MONOM_HPP__

#include <math.h>
#include <string>
#include <sstream>
#include <util/common/util.hpp>
#include "polynomdefs.hpp"
//#include "polyutil.hpp"
#include <util/common/interval.hpp>


/**
  * Basic class to support monomials
  * FT - variables type
  */
template <typename FT> class Monom {
  public:
    /**
      * Empty constructor
      */
    Monom()
    {
      mCoe = -1;
      mNvars = -1;      
    }

    /**
      * Constructor
      * @param nvars number of variables
      * @param coe scalar multiplier
      * @param deg array of variables degrees
      */
    Monom(unsigned int nvars, FT coe, const unsigned int* deg)
    {
      BNB_ASSERT(nvars <= MAX_POLYNOM_VARS);
      mNvars = nvars;
      mCoe = coe;      
      for(int i = 0; i < mNvars; i ++) {
	mDeg[i] = deg[i];
      }
    }


    /**
      * @return coefficient
      */
    FT getCoe() const
    {
      return mCoe;
    }

    /**
      * @return array of variables powers
      */
    const unsigned int* getDeg() const
    {
      return mDeg;
    }


    /**
      * Retrieve number of variables
      * @return number of variables
      */
    int getNvars() const 
    {
      return mNvars;
    }

    /**
      * Retrieve polynom value
      *
      * @param vars 
      * @return polynom value
      */
    FT getValue(const FT* vars) const
    {
      FT v = mCoe;
      for(int i = 0; i < mNvars; i ++) {
	FT u = pow(vars[i], mDeg[i]);
	v *= u;
      }
      return v;
    }

    /**
      * Retrieve monom value using cached matrix of variable degrees
      * 
      * FORMAT:
      * m1 1 x1 x1^2 ... x1^m1
      * m2 1 x2 x2^2 ... x2^m2
      * ...
      * mn 1 xn xn^2 ... xn^mn
      * where mi - number of precalculated degrees for xi
      *
      * @param varsm matrix of variables degrees
      * @return polynom value
      */
    FT getValueCache(FT* varsm) const
    {
      FT v = mCoe;
      const FT *vrp = varsm;
      const unsigned int *deg = mDeg;
      for(int i = 0; i < mNvars; i ++) {
	unsigned int d = (*deg) + 1;
	v *= vrp[d];
	vrp += DEG_MATRIX_ROW_LEN;
	deg ++;
      }
      return v;
    }


    /**
      * Parforms Partial derivation
      * @param nvar variable to differentiate
      */
    void deriv(unsigned int nvar)  
    {
      BNB_ASSERT(nvar >= 0);
      BNB_ASSERT(nvar < mNvars);
      if(mCoe != 0) {
	if(mDeg[nvar] == 0) {
	  mCoe = 0;
	} else {
	  mCoe *= mDeg[nvar];
	  for(int i = 0; i < mNvars; i ++) {
	    mDeg[i] -= ((i == nvar) ? 1 : 0);
	  }
	}
      }
    }
    /**
     * Retrieve lower and upper bounds
     *
     * @param lvars lower bounds for variables
     * @param uvars upper bounds for variables
     * @param lbound obtained lower bound
     * @param ubound obtained upper bound
     */
    FT getBounds(const FT* lvars, const FT* uvars, FT* lbound, FT* ubound) const
    {
      *lbound = mCoe;
      *ubound = mCoe;
      if(mCoe != 0) {
	for(int i = 0; i < mNvars; i ++) {
	  FT lb, ub;
	  if(mDeg[i] != 0) {
	    if(mDeg[i] == 1) {
	      lb = lvars[i];
	      ub = uvars[i];
	    } else {
	      BNBInterval<FT>::degree(mDeg[i], lvars[i], uvars[i], &lb, &ub);
	    }
	    BNBInterval<FT>::mult(*lbound, *ubound, lb, ub, lbound, ubound);
	  }
	}
      }
    }

    /**
      * Retrieve lower and upper bound using cached information
      *
      * FORMAT:
      * m1 a1 b1 l11 u11 a1^2 b1^2 l12 u12 ... a1^m1 b1^m1 l1m1 u1m1
      * m2 a2 b2 l21 u21 a2^2 b2^2 l22 u22 ... a2^m2 b2^m2 l2m2 u2m2
      * ...
      * mn an bn ln1 un1 an^2 bn^2 ln2 u2n ... an^mn bn^mn lnmn unmn
      *
      * where mi - number of precalculated interval degrees bound
      * [ai, bi] - bounds for xi
      * [lij, uij] - interval bounds for xi^j 
      *
      * @param boundsm matrix with precalculated degrees bounds
      * @param lbound resulting left bound
      * @param ubound resulting upper bound
      */
   void getBoundsCache(FT* boundsm, FT* lbound, FT* ubound) const 
   {
     *lbound = mCoe;
     *ubound = mCoe;
     FT* vri = boundsm;
     for(int i = 0; i < mNvars; i ++) {
       unsigned int d = mDeg[i];
       int ind = d * 4;
       BNBInterval<FT>::mult(*lbound, *ubound, vri[ind + 3], vri[ind + 4], lbound, ubound);
       vri += DEG_INTERVAL_MATRIX_ROW_LEN;
     }
   }

  private:
    FT mCoe;
    int mNvars;
    unsigned int mDeg[MAX_POLYNOM_VARS];
};
#endif
