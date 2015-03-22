#ifndef __LINOBJECTIVE_HPP__
#define __LINOBJECTIVE_HPP__

#include <problems/optlib/objective.hpp>

template <typename FT> class LinObjective : virtual public Objective <FT> 
{  
  public:

    LinObjective()
    {
      mCoeff = NULL;
    }

    /**
      * Set array of linear functions coefficients
      * @param coeff coefficients (a1,a2,...,an,a0)
      */
    void setCoeff(FT* coeff)
    {
      mCoeff = coeff;
    }

    /**
      * Retrieve the array of coefficients
      * @return array of coefficients
      */
    FT* getCoeff() const
    {
      BNB_ASSERT(mCoeff);
      return mCoeff;
    }

    FT func(const FT* x)
    {
      BNB_ASSERT(mCoeff);
      int n = Objective<FT>::getDim();
      FT v = mCoeff[n];
      for(int i = 0; i < n; i ++) 
	v += mCoeff[i] * x[i];
      return v;
    }

  private:

    FT* mCoeff;

};

#endif
