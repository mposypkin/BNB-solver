#ifndef __CONSDISCARDER__HPP__
#define __CONSDISCARDER__HPP__

#include <problems/go/go.hpp>
#include <util/common/vec.hpp>
#include <util/common/gvector.hpp>

class ConsDiscarder : public GOFactory<double>::Discarder
{
  public:
    ConsDiscarder()
    {
      mC = NULL;
    }
  
    ~ConsDiscarder()
    {
      if(mC)
        free(mC);
    }
  
    /**
     * Setup dimension
     * @param dim 
     */
    void setDim(int dim)
    {
      mDim = dim;
      mC = (double*) realloc(mC, mDim * sizeof(double));
    }
  
    
    void discard(GOFactory<double>::Set& s, GVector<GOFactory<double>::Set>& sv,GVector<GOFactory<double>::Solution>& solv, GOFactory<double>::ValueType record, BNBBranchInfo* info)
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
