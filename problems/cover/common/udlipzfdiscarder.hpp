#ifndef __UDLIPSFDICARDER_HPP__
#define __UDLIPSFDICARDER_HPP__

#include "go.hpp"
#include "lipzobjective.hpp"

/**
 * A discarder based on a Lipzic formula for a function
 * for uni-dimensional function
 */
template <typename FT> class UdLipzFuncDiscarder : public GOFactory<FT>::Discarder 
{
  
  public:
    
    UdLipzFuncDiscarder()
    {
      mObj = NULL;
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
    
    bool discard( typename GOFactory<FT>::Set& s, typename GOFactory<FT>::ValueType record)
    {
      bool rv;
      FT l, v, r;
      
      BNB_ASSERT(mObj);
      BNB_ASSERT(mC);
      l = getLipz(s);      
      r = 0;
      for(int i = 0; i < mObj->getDim(); i ++) {
        mC[i] = 0.5 * (s.mA[i] + s.mB[i]);        
        r += BNBSQR(0.5 * (s.mB[i] - s.mA[i]));
      }
      r = sqrt(r);
      v = mObj->func(mC);
      rv = ((v - l * r) > record - mEps);
      return rv;
    }
    
  private:
    
    FT getLipz(const typename GOFactory<FT>::Set& s) const 
    {
      return mObj->getFuncLipzConstant(s.mA.getPtr(), s.mB.getPtr());
    }
    
    FT mEps;
    FT* mC;
    LipzObjective<FT>* mObj;
};



#endif