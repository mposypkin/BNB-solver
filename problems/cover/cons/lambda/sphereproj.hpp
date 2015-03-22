#ifndef __SPHEREPROJ_HPP__
#define __SPHEREPROJ_HPP__

#include <util/common/vec.hpp>
#include "conssm.hpp"

class SphereProjector : public ConsSolutionMaker::Projector {

  public:
    SphereProjector(int dim) 
    {
      mDim = dim;
      mRadius = 1.;
    }
    
    void project(double* x, double* y) 
    {
      int n;
      double r;
      
      n = mDim;      
      r = VecUtils::vecNormTwo<double>(n, x);
      if(r != 0.) {
        double factor = mRadius / r;
        VecUtils::vecMult<double>(n, x, factor, y);
      } else {
        for(int i = 1; i < n; i ++) {
          y[i] = 0.;
        }
        y[0] = 1.;
      }
    }
  private:
    int mDim;
    double mRadius;
};
#endif
