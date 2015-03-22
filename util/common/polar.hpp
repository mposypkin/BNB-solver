#ifndef __POLAR_HPP__
#define __POLAR_HPP__

#include <stdlib.h>
#include <math.h>

class Polar 
{
  public:
  /**
   * Translate polar to cartesian coordinates
   * @param n dimension
   * @param r shpere radius
   * @param phi angles
   * @param x resulting cart coordinates
   */
  template <typename FT> static void polar2cart(int n, FT r, const FT* phi, FT* x)
  {
    for(int i = 0; i < (n-1); i ++) {
      x[i] = r * cos(phi[i]); 
    }
    x[n-1] = r;
    for(int i = 1; i < n; i ++) {
      double s = sin(phi[i-1]); 
      for(int j = i; j < n; j ++)
        x[j] *= s;
    }
  }
  
  /**
   * Evaluates function Gradient w.r.t. polar angles
   * @param n dimension
   * @param r radius
   * @param phi angles
   * @param cgrad cartesian grad at the point
   * @param grad resulting grad compoments for angles
   */
  template <typename FT> static void polarAngleGrad(int n, FT r, const FT* phi, const FT* cgrad, FT* grad) 
  {
    FT* si = (FT*)malloc((n-1) * sizeof(FT));
    FT* co = (FT*)malloc((n-1) * sizeof(FT));
    for(int i = 0; i < (n - 1); i ++) {
      si[i] = sin(phi[i]);
      co[i] = cos(phi[i]);
    }
    for(int i = 0; i < (n - 1); i ++) {
      FT v = 0., t = -cgrad[i];
      for(int j = 0; j <= i; j ++) 
        t *= si[j];
      v += t;
      for(int j = (i + 1); j < (n - 1); j ++) {
        t = cgrad[j] * co[j];
        for(int k = 0; k < j; k ++) {
          if(k == i)
            t *= co[k];
          else
            t *= si[k];
        }
        v += t;
      }
      t = cgrad[n-1];
      for(int j = 0; j < (n - 1); j ++) {
        if(j == i)
          t *= co[j];
        else
          t *= si[j];
      }
      v += t;
      grad[i] = r * v;
    }
    free(si);
    free(co);
  }
};

#endif
