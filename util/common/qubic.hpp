#ifndef __QUBIC_HPP__
#define __QUBIC_HPP__

#include <stdio.h>
#include <math.h>
#include "bnberrcheck.hpp"
#include "util.hpp"

/**
 * Solve qubic equations numerically
 */

template <typename FT> class Qubic {
  
  public: 
    
    Qubic(FT a, FT b, FT c, FT d, FT eps = 0.000001)
    {
      BNB_ASSERT(a);
      mA = a;
      mB = b;
      mC = c;
      mD = d;
      mEps = eps;
      printf("COEF: %lf, %lf, %lf, %lf\n", mA, mB, mC, mD);
    }
    
  /**
   * Solves qubic equation ax^3 + bx^2 + cx + d = 0
   * @param roots roots found
   * @return number of roots (should be 1 or 3)
   */
  int solve(FT* roots)
  {
    double q = (mA < 0) ? -1 : 1; 
    mA *= q;
    mB *= q;
    mC *= q;
    mD *= q;
    FT D = mB * mB - 3. * mA * mC;
    FT T = BNBMAX((BNBABS(mB) + BNBABS(mC) + BNBABS(mD)) / BNBABS(mA), 1.);
    int nr;
    if(D <= 0.) {
        nr = 1;
        findRoot(-T, T, roots);
    } else if(D > 0.) {
      FT Q = sqrt(D);
      FT x1 = (-mB - Q) / (3. * mA);
      FT x2 = (-mB + Q) / (3. * mA);
      FT f1 = f(x1);
      FT f2 = f(x2);
      if(f1 < 0.) {
        nr = 1;
        findRoot(x2, T, roots);
      } else if(f1 == 0.) {
        roots[0] = x1;
        if(f2 == 0.) {
          nr = 1;
        } else {
          nr = 2;
          findRoot(x2, T, roots + 1);
        }
      } else if(f1 > 0.) {
        findRoot(-T, x1, roots);
        if(f2 > 0.) {
          nr = 1;
        } else if(f2 == 0.) {
          nr = 2;
          roots[1] = x2;
        } else if(f2 < 0.) {
          nr = 3;
          findRoot(x1, x2, roots + 1);
          findRoot(x2, T, roots + 2);
        }
      }
    }
    mA *= q;
    mB *= q;
    mC *= q;
    mD *= q;
    return nr;
  }

  
  double findMinimum(double a, double b)
  {
    FT Fa = f(a);
    FT Fb = f(b);
    FT Fmin = BNBMIN(Fa, Fb);
    FT D = mB * mB - 3. * mA * mC;
    if(D > 0) {
      FT Q = sqrt(D);
      FT x1 = (-mB - Q) / (3. * mA);
      FT x2 = (-mB + Q) / (3. * mA);
      if((a < x1) && (x1 < b)) {
        FT f1 = f(x1);
        Fmin = BNBMIN(f1, Fmin);
      }
      if((a < x2) && (x2 < b)) {        
        FT f2 = f(x2);
        Fmin = BNBMIN(f2, Fmin);
      }
    }      
    printf("min = %lf at [%lf, %lf]\n", Fmin, a, b);
    return Fmin;
  }
  
  private:
    
    FT f(FT x)
    {
      return mA * x * x * x + mB * x * x + mC * x + mD;
    }
    
    void findRoot(double a, double b, double* root)
    {
      printf("[%lf, %lf]\n", a, b);
      int i = 0;
      for(;;) {
        double x = 0.5 * (a + b);
        double res = f(x);
        if(BNBABS(res) < mEps) {
          *root = x;          
          break;
        } else if(res * f(a) < 0){
          b = x;
        } else {
          a = x;
        }      
        i ++;
      }
      printf("i = %d, root = %lf\n", i, *root);
    }
    
    FT mA;
    FT mB;
    FT mC;
    FT mD;
    FT mEps;
};

#endif
