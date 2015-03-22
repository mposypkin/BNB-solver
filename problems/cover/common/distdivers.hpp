#ifndef __DISTDIVERS_HPP__
#define __DISTDIVERS_HPP__

#include "go.hpp"

template <typename FT> class DistDivers : public  BNBDivers < typename GOFactory<FT>::Solution > 
{
  public:
    
    DistDivers()
    {
      mEps = 0.;
    }
    
  /**
   * Checks whether two solution are really different (belongs to different domains)
   * @param s1 first solution
   * @param s2 second solution
   * @return true if same false otherwise
   */
    virtual bool same(const typename GOFactory<FT>::Solution s1, const typename GOFactory<FT>::Solution s2) 
    {
      FT nor = 0.;
      int dim = s1.mDim;
      for(int i = 0; i < dim; i ++) {
        nor = BNBMAX(nor, BNBABS(s1.mX[i] - s2.mX[i]));
      }
      return nor <= mEps;
      /** TMP **/
      //return ((nor <= mEps) || BNBABS(s1.mFval - s2.mFval) < 0.0001);
    }

    /**
     * Setup difference measure
     * @param eps difference measure
     */
    void setEps(FT eps)
    {
      mEps = eps; 
    }
    
  private:
    
    
    FT mEps;
};

#endif
