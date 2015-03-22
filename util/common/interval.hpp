#ifndef _INTERVAL_HPP_
#define _INTERVAL_HPP_

#include <math.h>
#include "bnberrcheck.hpp"
#include "util.hpp"

/**
 * Basic interval arithmetic
 */
template <typename flot> struct BNBInterval {

    static void sum(flot xmin, flot xmax, flot ymin, flot ymax, flot* zmin, flot* zmax)
    {
      *zmin = xmin + ymin;
      *zmax = xmax + ymax;
    }
    
    static void diff(flot xmin, flot xmax, flot ymin, flot ymax, flot* zmin, flot* zmax)
    {
      *zmin = xmin - ymax;
      *zmax = xmax - ymin;
    }
    
    static void sqr(flot xmin, flot xmax, flot* ymin, flot* ymax)
    {
      flot a, b;
      a = BNBSQR(xmin);
      b = BNBSQR(xmax);
      if((xmin <= 0.) && (xmax >= 0.)) 
        *ymin = 0.;
      else
        *ymin = BNBMIN(a, b);
      *ymax = BNBMAX(a, b);
    }
    
    static void degree(int n, flot xmin, flot xmax, flot* ymin, flot* ymax)
    {
      flot a = 1., b = 1.;
      for(int i = 1; i <= n; i ++) {
        a *= xmin;
        b *= xmax;
      }
      if(n % 2) {
        *ymin = a;
        *ymax = b;
      } else {
        if((xmin <= 0.) && (xmax >= 0.)) 
          *ymin = 0.;
        else
          *ymin = BNBMIN(a, b);
        *ymax = BNBMAX(a, b);
      }        
    }
    
    static void mult(flot xmin, flot xmax, flot ymin, flot ymax, flot* zmin, flot* zmax)
    {
      flot e1, e2, e3, e4, z1, z2;
      e1 = xmin * ymin;
      z1 = e1;
      z2 = e1;
      e2 = xmin * ymax;
      z1 = BNBMIN(z1, e2);
      z2 = BNBMAX(z2, e2);
      e3 = xmax * ymin;
      z1 = BNBMIN(z1, e3);
      z2 = BNBMAX(z2, e3);
      e4 = xmax * ymax;
      z1 = BNBMIN(z1, e4);
      z2 = BNBMAX(z2, e4);
      *zmin = z1;
      *zmax = z2;
      /*
      if((xmin >=0) && (ymin >= 0)) {
        *zmin = xmin * ymin;
        *zmax = xmax * ymax;
      } else if((xmin >= 0) && (ymin < 0) && (ymax > 0)) {
        *zmin = xmax * ymin;
        *zmax = xmax * ymax;    
      } else if((xmin >= 0) && (ymax <= 0)) {
        *zmin = xmax * ymin;
        *zmax = xmin * ymax;
      } else if((xmin < 0) && (xmax > 0) && (ymin >= 0)) {
        *zmin = xmin * ymax;
        *zmax = xmax * ymax;
      } else if((xmin < 0) && (xmax > 0) && (ymin < 0) && (ymax > 0)) {
        *zmin = BNBMIN(xmax * ymin, xmin * ymax);
        *zmax = BNBMAX(xmin * ymin, xmax * ymax);
      } else if((xmin < 0) && (xmax > 0) && (ymax <= 0)) {
        *zmin = xmax * ymin;
        *zmax = xmin * ymin;
      } else if((xmax <= 0) && (ymin >= 0)) {
        *zmin = xmin * ymax;
        *zmax = xmax * ymin;
      } else if((xmax <= 0) && (ymin < 0) && (ymax > 0)) {
        *zmin = xmin * ymax;
        *zmax = xmin * ymin;
      } else if((xmax <= 0) && (ymax <= 0)) {
        *zmin = xmax * ymax;
        *zmax = xmin * ymin;
      } else {
        printf("[%lf, %lf] x [%lf, %lf]\n", xmin, xmax, ymin, ymax);
        BNB_ERROR_REPORT("Illegal combination in interlval multiplication");
      }
      */
    }
    
    static void sin(flot xmin, flot xmax, flot* ymin, flot* ymax)
    {
      int k, l;
      bool p2 = false;
      bool p32 = false;
      
      k = (int) ceil(xmin * M_2_PI);
      l = (int) floor(xmax * M_2_PI);
      for(int i = k; i <= l; i ++) {
        if((i % 4) == 1)
          p2 = true;
        else if((i % 4) == 3)
          p32 = true;
        if(p2 && p32)
          break;
      }
      if(p2)
        *ymax = 1.;
      else
          *ymax = BNBMAX(::sin(xmin), ::sin(xmax));
      if(p32)
        *ymin = -1.;
      else 
        *ymin = BNBMIN(::sin(xmin), ::sin(  xmax));
    }
    
    static void cos(flot xmin, flot xmax, flot* ymin, flot* ymax)
    {
      flot a, b;
      a = M_PI_2 - xmax;
      b = M_PI_2 - xmin;
      sin(a, b, ymin, ymax);       
    }
    
    static void polynom(int n, flot* coeff, flot xmin, flot xmax, flot* ymin, flot* ymax)
    {
      double a = 0., b = 0.;
      for(int i = 0; i <= n; i ++) {
        mult(a, b, xmin, xmax, &a, &b);
        sum(a, b, coeff[i], coeff[i], &a, &b);
      }
      *ymin = a;
      *ymax = b;
    }
};

#endif
