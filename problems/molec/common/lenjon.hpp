#ifndef _LENJON_HPP_
#define _LENJON_HPP_
/**
 * Lennard Jones potential
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file lenjon.hpp
 */

#include <math.h>
#include <util/common/interval.hpp>
#include <util/common/vec.hpp>

#include "potent.hpp"
#include "molecutils.hpp"


template <class FloatType> class LenJon : public Potential <FloatType>
{
  public:
    
    LenJon()
    {
      printf("LJ-Potential\n");
    }

    FloatType potent(FloatType dsq)
    {
      /*
      FloatType x, v;      
      x = dsq * dsq * dsq;
      v = 1./BNBSQR(x) - 2./x; 
      return v;
      */
      FloatType x, v;      
      x = 1./(dsq * dsq * dsq);
      v = x * (x - 2.); 
      return v;
      
    }

    FloatType df(FloatType dsq)
    {
      /*
      FloatType x2, x3, v;
      x2 = dsq * dsq;
      x3 = x2 * dsq;
      v =6. * (1./(x2*x2) - 1./(x2*x2*x3));
      return v;
      */
      FloatType x2, x3, v;
      x2 = dsq * dsq;
      x3 = x2 * dsq;
      v =6. * (1./(x2*x2)) * (1. - 1./x3);
      return v;

    }
    
    void fandf(FloatType dsq, FloatType* fval, FloatType* der)
    {
      FloatType x, y, v, d;
      x = 1./dsq;
      y = x * x * x;
      v = y * (y - 2.);
      d = -6. * (v + y) * x;
      *fval = v;
      *der = d;
    }
  
    FloatType ddf(FloatType dsq)
    {
      FloatType v;
      FloatType x2, x3, x5, x8;
      
      x2 = dsq * dsq;
      x3 = x2 * dsq;
      x5 = x2 * x3;
      x8 = x5 * x3;
      v  = 42. / x8 - 24. / x5;
      return v;
    }
    
    FloatType dfmax ()
    {
      return pow(7./4., 1./3.);
    }
    
    FloatType ddfmax ()
    {
      return pow(14./5., 1./3.);
    }

    FloatType leftbound() 
    {
      return 0.25;
    }

    
    /*
    void boundGrad(int N, int D, FloatType* a, FloatType* b, FloatType* gmin, FloatType* gmax)
    {
      int n;
      n = D * N;
      for(int i = 0; i < n; i ++) {
        gmin[i] = 0;
        gmax[i] = 0;
      }
      for(int I = 0; I < N; I ++) {
        int i;
        i = I * D;
        for(int J = I + 1; J < N; J ++) {
          int j;
          FloatType qmin, qmax, x, y; 
          j = J * D;          
          qmin = evalSqrMinDistance(D, a + i, b + i, a + j, b + j);
          qmax = evalSqrMaxDistance(D, a + i, b + i, a + j, b + j);
          if(qmin >= mQ1){
            x = df(qmax);
            y = df(qmin);
          } else if(qmax <= mQ1) {
            x = df(qmin);
            y = df(qmax);
          } else {
            x = BNBMIN(df(qmin), df(qmax));
            y = df(mQ1);
          }

          for(int k = 0; k < D; k ++) {
            int ik, jk;
            FloatType xikl, xiku, xjkl, xjku, c, d;

            
            ik = I * D + k;
            jk = J * D + k;
            xikl = a[ik];
            xiku = b[ik];
            xjkl = a[jk];
            xjku = b[jk];
            BNBInterval<FloatType>::diff(xikl, xiku, xjkl, xjku, &c, &d);
            BNBInterval<FloatType>::mult(c, d, x, y, &c, &d);
            BNBInterval<FloatType>::sum(gmin[ik], gmax[ik], c, d, gmin + ik, gmax + ik); 
            BNBInterval<FloatType>::diff(xjkl, xjku, xikl, xiku, &c, &d);
            BNBInterval<FloatType>::mult(c, d, x, y, &c, &d);
            BNBInterval<FloatType>::sum(gmin[jk], gmax[jk], c, d, gmin + jk, gmax + jk); 
          }
        }
      }
      FloatType alpha = 1.;
      VecUtils::vecMult(n, gmin, alpha * 2., gmin);
      VecUtils::vecMult(n, gmax, alpha * 2., gmax);
    }
    */
};
#endif
