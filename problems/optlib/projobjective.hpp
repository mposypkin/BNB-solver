#ifndef __PROJOBJECTIVE_HPP__
#define __PROJOBJECTIVE_HPP__

#include <util/common/bnberrcheck.hpp>
#include "projector.hpp"
#include "objective.hpp"


template <typename FT> class ProjectedObjective : public Objective <FT>
{
  public:
    
  ProjectedObjective(Objective<FT>* obj, Projector<FT>* proj)
  {
    mObjective = obj;
    Objective<FT>::setDim(mObjective->getDim());
    mProjector = proj;
  }
    
  /**
   * The objective function itself
   *
   * @param x argument
   *
   * @return function value
   *
   */
  FT func(FT* x)
  {
    mProjector->project(mObjective->getDim(), x);
    return mObjective->func(x);
  }

  /**
   * Gradient
   * 
   * @param x argument
   * @param g gradient
   */
  void grad(FT* x, FT* g)
  {
    mProjector->project(mObjective->getDim(), x);
    mObjective->grad(x, g);
    mProjector->project(mObjective->getDim(), g);
  }
  
  /**
   * Calculate Hessian matrix
   * @param x point
   * @param H hessian ((n + 1) x n)
   */
  virtual void hess(FT* x, FT* H)
  {
    BNB_ERROR_REPORT("Hessian is not supported for projected potential");
  }

  
  private:
    
    Objective<FT>* mObjective;
    Projector<FT>* mProjector;

};
#endif
