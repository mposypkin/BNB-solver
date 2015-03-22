#ifndef __SPHEREDISC_HPP__
#define __SPHEREDISC_HPP__

#include <problems/cover/common/go.hpp>
#include <util/common/vec.hpp>

class SphereDiscarder : public GOFactory<double>::Discarder
{
  public:
    SphereDiscarder(int dim)
    {
      mDim = dim;
      mC = (double*) malloc(mDim * sizeof(double));
    }
  
    ~SphereDiscarder()
    {
      if(mC)
        free(mC);
    }
  
    
    void discard(GOFactory<double>::Set& s, GVector<GOFactory<double>::Set>& sv, GVector<GOFactory<double>::Solution>& solv, GOFactory<double>::ValueType record, BNBBranchInfo* info)
    {
      bool rv;
      double dmin, dmax;
      int sig;
      dmin = 0.;
      dmax = 0.;
      for(int i = 0; i < mDim; i ++) {
        if(s.mA[i] > 0) {
          dmin += s.mA[i] * s.mA[i];
        } else if(s.mB[i] < 0) {
          dmin += s.mB[i] * s.mB[i];          
        } 
        dmax += BNBMAX(s.mA[i] * s.mA[i], s.mB[i] * s.mB[i]);
      }
      if((dmax < 1.) || (dmin > 1.))
        rv = true;
      else 
        rv = false;
      if(!rv)
        sv.push_back(s);
      else 
        info->mDiscardedByConstraints ++;
    }
    
  private:
    int mDim;
    double* mC;
};

#endif
