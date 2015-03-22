#ifndef _MORS_HPP_
#define _MORS_HPP_
/**
 * Morse potential
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file mors.hpp
 */

#include <math.h>
#include <util/common/interval.hpp>
#include <util/common/vec.hpp>

#include "potent.hpp"
#include "molecutils.hpp"

template <class FloatType> class Mors : public Potential <FloatType>
{
public:
/**
 * The constructor
 *
 * @param rho the rho parameter of the Morse potential
 */
  Mors(FloatType rho)
  {
    mRho = rho;
    mQ0 = 0.;
    mQ1 = BNBSQR(1. - log(0.5) / mRho);
    printf("mQ1 = %lf\n", mQ1);
    printf("df(0) = %lf\n", df(0));
  }
  
//   FloatType potent(FloatType dsq)
//   {
//     FloatType x, v;
//       
//     if(dsq >= mQ0) {
//       v = BNBSQR(exp(mRho * (1. - sqrt(dsq)))-1.) - 1.; 
//     } else {
//       FloatType k, b;
//       v = potent(mQ0) + (sqrt(dsq) - sqrt(mQ0)) * df(mQ0);
//     }
//     return v;
//   }

  FloatType potent(FloatType dsq)
  {
    FloatType r, e, v;
    r = sqrt(dsq);
    e = exp(mRho * (1. - r));
    v = BNBSQR(e - 1.) - 1.; 
    return v;
  }

  FloatType df(FloatType dsq)
  {
    FloatType r, e, v;
    r = sqrt(dsq);
    e = exp(mRho * (1. - r));
    v = (mRho * e * (1. - e)) / r;
    return v;
  }
  
  void fandf(FloatType dsq, FloatType* fval, FloatType* der)
  {
    FloatType r, e, v, d;
    r = sqrt(dsq);
    e = exp(mRho * (1. - r));
    v = BNBSQR(e - 1.) - 1.; 
    d = (mRho * e * (1. - e)) / r;
    *fval = v;
    *der = d;
  }

  FloatType ddf(FloatType q) 
  {
    FloatType E, r, v;
    
    r = sqrt(q);
    E =  exp(mRho * (1. - r));
    v = ((mRho * E) / (2 * q)) * (mRho * (2 * E - 1) + (E - 1)/r);
    //printf("v = %lf, e = %lf, r = %lf\n", v, E, r);
    return v;
  }
  
  FloatType dfmax()
  {
    return BNBSQR(1. - log(0.5) / mRho);
  }
  
  FloatType ddfmax()
  {
    BNB_ERROR_REPORT("argmax for 2nd derivative is not yet implemented");
  }
  

  FloatType leftbound()
  {
    return 0.0;
  }
  
  FloatType df_old(FloatType dsq)
  {
    FloatType r, e;

    r = sqrt(dsq);
    e = exp(mRho * (1. - r));
    return (mRho * e * (1. - e)) / r;
  }
  
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
    VecUtils::vecMult(n, gmin, 2., gmin);
    VecUtils::vecMult(n, gmax, 2., gmax);
  }

  
  void boundGrad_old(int N, int D, FloatType* a, FloatType* b, FloatType* gmin, FloatType* gmax)
  {
    const FloatType Rmin = 0.5  ;
        
    for(int L = 0; L < N; L ++) {
      int l;
      l = L * D;
      for(int k = 0; k < D; k ++) {
        gmin[l + k] = 0.;
        gmax[l + k] = 0.;
      }
      for(int I = 0; I < N; I ++) {
        if(I != L) {
            int i;
            FloatType  qmin, qmax, rmin, rmax, 
            Ea, Eb, Ga, Gb, Gmin, Gmax, gl, gu;
            i = I * D;
            qmin = evalSqrMinDistance(D, a + l, b + l, a + i, b + i);
            rmin = sqrt(qmin);
            rmin = BNBMAX(rmin, Rmin);
            qmax = evalSqrMaxDistance(D, a + l, b + l, a + i, b + i);
            rmax = sqrt(qmax);
            Eb = exp(mRho * (1. - rmin));
            Gb = Eb * (1. - Eb);
            Ea = exp(mRho * (1. - rmax));
            Ga = Ea * (1. - Ea);
            Gmin = BNBMIN(Ga, Gb);
            if(Eb <= 0.5) 
              Gmax = Gb;
            else if (Ea >= 0.5)
              Gmax = Ga;
            else
              Gmax = 0.25;
            for(int k = 0; k < D; k ++) {
              FloatType fmin, fmax, xmin, xmax, cmin, cmax;
              cmin = 0;
              cmax = 0;
              for(int m = 0; m < D; m ++) {
                if(m != k) {
                  FloatType lb, ub;
                  BNBInterval<FloatType>::diff(a[l + m], b[l + m], a[i + m], b[i + m], &lb, &ub);
                  BNBInterval<FloatType>::sqr(lb, ub, &lb, &ub);
                  BNBInterval<FloatType>::sum(cmin, cmax, lb, ub, &cmin, &cmax); 
                }
              }
              BNBInterval<FloatType>::diff(a[l + k], b[l + k], a[i + k], b[i + k], &xmin, &xmax);
              if(xmin >= 0.) {
                if(cmax == 0.)
                  fmin = 1.;
                else
                  fmin = xmin / sqrt(xmin * xmin + cmax);
                if(cmin == 0.)
                  fmax = 1.;
                else
                  fmax = xmax / sqrt(xmax * xmax + cmin);
              } else if(xmax > 0.) {
                if(cmin == 0.)
                  fmin = -1.;
                else
                  fmin =  xmin / sqrt(xmin * xmin + cmin);
                if(cmin == 0.)
                  fmax = 1.;
                else
                  fmax =  xmax / sqrt(xmax * xmax + cmin);
              } else {
                if(cmin == 0.)
                  fmin = -1.;
                else
                  fmin =  xmin / sqrt(xmin * xmin + cmin);
                if(cmax == 0.)
                  fmax = -1.;
                else
                  fmax =  xmax / sqrt(xmax * xmax + cmax);
              }            
              BNBInterval<FloatType>::mult(fmin, fmax, Gmin, Gmax, &gl, &gu);
              BNBInterval<FloatType>::sum(gl, gu, gmin[l + k], gmax[l + k], 
                                     gmin + l + k, gmax + l + k);
            }
        }
      }
    }
    int n;
    n = D * N;
    VecUtils::vecMult(n, gmin, 2. * mRho, gmin);
    VecUtils::vecMult(n, gmax, 2. * mRho, gmax);
  }

private:
  
  FloatType mRho;
  
  FloatType mQ1;
  
  FloatType mQ0;
};

#endif
