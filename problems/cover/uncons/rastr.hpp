#ifndef __RASTR_HPP__
#define __RASTR_HPP__

#include <math.h>
#include <problems/optlib/objective.hpp>

/**
  * Rastringin function
  */
template <class FT> class Rastr: public Objective<FT> {
  public:
    
    Rastr(int n, FT a)
    {
      Objective<FT>::mDim = n;
      mA = a;
    }

    FT func(const FT* x)
    {
      FT v;
      int n;
      n = Objective<FT>::mDim;
      v = n * mA;
      for(int i = 0; i < n; i ++) {
        v += x[i] * x[i] - mA * cos(2 * M_PI * x[i]);
      }
      return v;
    }
        
    void grad(const FT* x, FT* g)
    {
      int n;
      n = Objective<FT>::mDim;
      for(int i = 0; i < n; i ++) {
        g[i] = 2 * x[i] + 2 * M_PI * mA * sin(2 * M_PI * x[i]);
      }
    }

    FT gradAndFunc(const FT* x, FT* g)
    {
      FT v;
      int n;
      n = Objective<FT>::mDim;
      v = n * mA;
      for(int i = 0; i < n; i ++) {
	v += x[i] * x[i] - mA * cos(2 * M_PI * x[i]);
	g[i] = 2 * x[i] + 2 * M_PI * mA * sin(2 * M_PI * x[i]);
      }
      return v;
    }

  private:

    FT mA;
};
#endif
