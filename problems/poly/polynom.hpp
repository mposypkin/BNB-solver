#ifndef __POLYNOM_HPP__
#define __POLYNOM_HPP__

#include <sstream>
#include <map>
#include <util/common/util.hpp>
#include "monom.hpp"
#include <util/common/fixedvector.hpp>
#include <util/common/smartptr.hpp>

/**
  * Basic class for plynomials support
  * FT - type of variables
  */
template <typename FT> class Polynom {
  public:
    typedef Monom<FT> MONOM;
    struct Options {

      /**
	* Use degree matrix cached evaluation
	*/
      static const unsigned int CACHE_EVAL = 1 << 1;

      /**
	* Use interval degree matrix cache for evaluation
	*/
      static const unsigned int CACHE_BOUND = 1 << 3;
    };



    /**
      * Constructor
      * @param nvars number of variables
      */
    Polynom()
    {
      mOptions = 0;
      mNvars = 0;
      //mOptions |= Options::CACHE_EVAL;
      mOptions |= Options::CACHE_BOUND;
      mMonomsArray.alloc(MAX_MONOMS);
      mMonoms.setCarrier(mMonomsArray, MAX_MONOMS);
    }


    /**
      * Append new monom to polynom
      * @param mon monom to add
      */
    void addMonom(const MONOM& mon)
    {
      mMonoms.push_back(mon);
      mNvars = BNBMAX(mNvars, mon.getNvars());
      BNB_ASSERT(mNvars <= MAX_POLYNOM_VARS);
    }

    /**
      * Copies polynom
      * @param poly polynom to copy from
      */
    void copy(const Polynom<FT>& poly)
    {
      mMonoms.clear();
      for(int i = 0; i < poly.getMonoms().size(); i ++) 
	addMonom(poly.getMonoms()[i]);
    }

    /**
      * Retrieve monoms vector
      * @return reference to monoms vector
      */
    const FixedVector<MONOM>& getMonoms() const
    {
      return mMonoms;
    }

    /**
      * Evaluate partial derivative
      * @param nvar derivative variable
      */
    void deriv(int nvar)
    {
      for(int i = 0; i < mMonoms.size();) {
	mMonoms[i].deriv(nvar);
	if(mMonoms[i].getCoe() == 0) {
	  mMonoms.erase(i);
	} else
	  i ++;
      }
    }

    /**
     * Retrieve polynom value
     *
     * @param vars
     * @return value
     */
    FT getValue(const FT* vars) const
    {
      if(mOptions & Options::CACHE_EVAL)
	return evalCache(vars);
      else
	return getNormalValue(vars);
    }

    /**
      * Retrieve polynom value based on the degree matrix caching
      * @param varsm degree matrix
      * @retun polynom value
      */
    FT getValueCache(FT* varsm) const
    {
      FT v = 0.;
      int ms = mMonoms.size();
      for(int i = 0; i < ms; i ++) {
	v += mMonoms[i].getValueCache(varsm);          
      }
      return v;
    }

    /**
      * Retrieve lower and upper bounds for polynomial values
      *
      * @param lvars lower bounds for variables
      * @param uvars upper bounds for variables
      * @param lbound obtained lower bound
      * @param ubound obtained upper bound
      */
    void getBounds(const FT* lvars, const FT* uvars, FT* lbound, FT* ubound) const
    {
      if(mOptions & Options::CACHE_BOUND)
	evalCachedBounds(lvars, uvars, lbound, ubound);
      else
	evalNormalBounds(lvars, uvars, lbound, ubound);
    }
    /**
     * Retrieve lower and upper bounds for polynomial values using cached matrix of precalculated bounds for degrees
     *
     * @param varsm precalculated cached matrix
     * @param lbound obtained lower bound
     * @param ubound obtained upper bound
     */
    void getBoundsCache(FT* varsm, FT* lbound, FT* ubound) const
    {
      *lbound = 0;
      *ubound = 0;
      int ms = mMonoms.size();
      for(int i = 0; i < ms; i ++) {
	FT l, u;
	mMonoms[i].getBoundsCache(varsm, &l, &u);
	*lbound += l;
	*ubound += u;
      }
    }


    /**
     * Retrieve number of variables
      * @return number of variables 
      */
    FT getNvars() const
    {
      return mNvars;
    }

    /**
      * Fill in variables matrix
      * @param nvars number of variables
      * @param vars array of variable values
      * @param varsm matrix of variables degrees
      */
    void fillMatrix(const FT* vars, FT* varsm) const
    {
      int n = mNvars;

      for(int i = 0; i < n; i ++) {
	FT* vri = varsm + i * DEG_MATRIX_ROW_LEN;
	vri[0] = 1;
	vri[1] = 1;
	vri[2] = vars[i];
      }
      int ms = mMonoms.size();
      for(int m = 0; m < ms; m ++) {
	for(int i = 0; i < n; i ++) {
	  FT* vri = varsm + i * DEG_MATRIX_ROW_LEN;
	  vri[0] = BNBMAX(vri[0], mMonoms[m].getDeg()[i]);
	}
      }
      for(int i = 0; i < n; i ++) {
	FT* vri = varsm + i * DEG_MATRIX_ROW_LEN;
	int d = vri[0];
	for(int j = 3; j <= d + 1; j ++) {
	  vri[j] = vri[j - 1] * vri[2];
	}
      }
    }

    /**
      * Fill in interval variables degree matrix
      * @param nvars number of variables
      * @param lbounds array of lower bounds
      * @param ubounds array of upper bounds
      * param varsm matrix of variables degrees
      */
    void fillIntervalMatrix(const FT* lbounds, const FT* ubounds, FT* varsm) const
    {
      typename std::vector< MONOM >::const_iterator m;
      int n = mNvars;
      for(int i = 0; i < n; i ++) {
	FT* vri = varsm + i * DEG_INTERVAL_MATRIX_ROW_LEN;
	vri[0] = 1;
	vri[1] = 1;
	vri[2] = 1;
	vri[3] = 1;
	vri[4] = 1;
	vri[5] = lbounds[i];
	vri[6] = ubounds[i];
	vri[7] = lbounds[i];
	vri[8] = ubounds[i];
      }
      int ms = mMonoms.size();
      for(int m = 0; m < ms; m ++) {
	for(int i = 0; i < n; i ++) {
	  FT* vri = varsm + i * DEG_INTERVAL_MATRIX_ROW_LEN;
	  vri[0] = BNBMAX(vri[0], mMonoms[m].getDeg()[i]);
	}
      }
      for(int i = 0; i < n; i ++) {
	FT* vri = varsm + i * DEG_INTERVAL_MATRIX_ROW_LEN;
	int d = vri[0];
	if(d > 1) {
	  FT l = vri[5];
	  FT u = vri[6];
	  for(int j = 2; j <= d; j ++) {
	    int jp = (j - 1) * 4;
	    int jn = j * 4;
	    vri[jn + 1] = vri[jp + 1] * l;
	    vri[jn + 2] = vri[jp + 2] * u;
	    if(j % 2) {
	      vri[jn + 3] = vri[jn + 1];
	      vri[jn + 4] = vri[jn + 2];
	    } else {
	      if((l <= 0.) && (u >= 0.)){
		vri[jn + 3] = 0;
	      } else {
		vri[jn + 3] = BNBMIN(vri[jn + 1], vri[jn + 2]);
	      }
	      vri[jn + 4] = BNBMAX(vri[jn + 1], vri[jn + 2]);
	    }
	  }
	}
      }
    }


  private:
    FT getNormalValue(const FT* vars) const
    {
      FT v = 0.;
      int ms = mMonoms.size();
      for(int i = 0; i < ms; i ++) {
	v += mMonoms[i].getValue(vars);
      }
      return v;
    }

    FT evalCache(const FT* vars) const
    {
      FT v;
      FT varsm[MAX_POLYNOM_VARS * DEG_MATRIX_ROW_LEN ];
      fillMatrix(vars, varsm);
      v = getValueCache(varsm);
      return v;
    }

    void evalNormalBounds(const FT* lvars, const FT* uvars, FT* lbound, FT* ubound) const
    {
      *lbound = 0;
      *ubound = 0;
      int ms = mMonoms.size();
      for(int i = 0; i < ms; i ++) {
	FT l, u;
	mMonoms[i].getBounds(lvars, uvars, &l, &u);
	*lbound += l;
	*ubound += u;
      }
    }

    void evalCachedBounds(const FT* lvars, const FT* uvars, FT* lbound, FT* ubound) const
    {
      FT varsm[MAX_POLYNOM_VARS * DEG_INTERVAL_MATRIX_ROW_LEN ];
      fillIntervalMatrix(lvars, uvars, varsm);
      getBoundsCache(varsm, lbound, ubound);
    }

    FixedVector<MONOM> mMonoms;
    SmartArrayPtr<MONOM> mMonomsArray;
    unsigned int mOptions;
    unsigned int mNvars;
};
#endif
