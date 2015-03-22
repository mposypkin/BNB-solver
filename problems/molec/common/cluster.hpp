#ifndef __CLUSTER_HPP__
#define __CLUSTER_HPP__

#include <problems/optlib/objective.hpp>
#include "potent.hpp"

template <typename FloatType> class Cluster : public Objective <FloatType> {
  
public:

    
  FloatType func(const FloatType* x)
  {
    return mPotential->totalPotential(mNAtoms, mSpaceD, x);
  }
  
  void grad(const FloatType* x, FloatType* g)
  {
    mPotential->totalGrad(mNAtoms, mSpaceD, x, g);
  }
  
  FloatType gradAndFunc(const FloatType* x, FloatType* g)
  {
    return mPotential->totalGradAndPot(mNAtoms, mSpaceD, x, g);
  }

  void hess(const FloatType* x, FloatType* H)
  {
    mPotential->hessian(mNAtoms, mSpaceD, x, H);
  }

  Potential<FloatType>* mPotential;

  int mNAtoms;
  
  int mSpaceD;
};

#endif
