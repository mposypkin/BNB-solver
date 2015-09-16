#ifndef __POLYOBJECTIVE_HPP__
#define __POLYOBJECTIVE_HPP__

#include <problems/optlib/objective.hpp>
#include <util/poly/polynom.hpp>
/**
  * Polynomial as an optimization objective
  */
template <typename FT> class PolyObjective : virtual public Objective <FT> {
  public:
    typedef Polynom<FT> POLYNOM;    
    /**
      * Constructor
      * @param poly polynomial
      */
    PolyObjective(POLYNOM* poly)
    {
      int n = poly->getNvars();
      mPoly = poly;
      Objective <FT>::setDim(n);
      for(int i = 0; i < n; i ++) {
	mGrad[i].copy(*poly);
	mGrad[i].deriv(i);
      }
      for(int i = 0; i < n; i ++) {
	for(int j = 0; j < n; j ++) {
	  mHess[i][j].copy(*poly);
	  mHess[i][j].deriv(i);
	  mHess[i][j].deriv(j);
	}
      }
    }

    FT func(const FT* x)
    {
      return mPoly->getValue(x);
    }

    void grad(const FT* x, FT* g)
    {
      int n = mPoly->getNvars();
      FT varsm[MAX_POLYNOM_VARS * DEG_MATRIX_ROW_LEN];
      mPoly->fillMatrix(x, varsm);
      for(int i = 0; i < n; i ++) {
	g[i] = mGrad[i].getValueCache(varsm);
      }
    }
    
    
    /**
     * Retrieve underline polynom
     * @return underline polynom
     */
    POLYNOM* getPoly() {
        return mPoly;
    }
    
    /**
     * Retrieve gradient vector
     * @return gradient vector
     */
    POLYNOM* getGradient() {
        return mGrad;
    }
    
    
    /**
     * Retrieve Hessian
     * @return hessian matrix as an array
     */
    POLYNOM (* getHessian())[MAX_POLYNOM_VARS] {
        return mHess;
    }
    
    
    
  protected:

    POLYNOM *mPoly;
    POLYNOM mGrad[MAX_POLYNOM_VARS];
    POLYNOM mHess[MAX_POLYNOM_VARS][MAX_POLYNOM_VARS];
};
#endif
