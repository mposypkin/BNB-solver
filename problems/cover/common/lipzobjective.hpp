#ifndef __LIPZOBJECTIVE_HPP__
#define __LIPZOBJECTIVE_HPP__

#include <problems/optlib/objective.hpp>
#include <util/common/utilmacro.hpp>

template <typename FT> class LipzObjective : virtual public Objective <FT> 
{  
  public:
  /**
   * Calculate a lipzic constant for a function for a given rectangle
   * 
   * @param a lower bound
   * @param b upper bound
   * @return Lipzic constant
   */
  virtual FT getFuncLipzConstant(const FT* a, const FT* b)
  {
    BNB_ERROR_REPORT("Lipzic constant for a function not implemented");
  }
  
  
  /**
   * Calculate bounds on eigenvalues for a given rectangle
   * 
   * @param a lower bound (in)
   * @param b upper bound (in)
   * @param emin spectrum lower bound (out)
   * @param emax spectrum upper bound (out)
   */
  virtual void getEigenBounds(const FT* a, const FT* b, FT& emin, FT& emax)
  {
    BNB_ERROR_REPORT("Eigen values bounds not implemented");
  }
  
  /**
   * Calculate Lipzic constant for a gradient component
   * in a given rectangle
   *    
   * @param a lower bound
   * @param b upper bound
   * @param i component number
   * @return Lipzic constant
   */
  virtual FT getGradCompLipzConstant(const FT* a, const FT* b, int i)
  {
    /*
    FT emin, emax;
    getEigenBounds(a, b, emin, emax);
    return BNBMAX(BNBABS(emin), BNBABS(emax));
    */
    BNB_ERROR_REPORT("Lipzic constant for gradient component not implemented");
  }
  
};

#endif
