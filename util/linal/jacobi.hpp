#ifndef __JACOBI_HPP_
#define __JACOBI_HPP_

#include <math.h>
#include <util/common/util.hpp>

#define ROTATE(a, i, j, k, l) {\
  FT g, h;\
  g = a[i * n + j];\
  h = a[k * n + l];\
  a[i * n + j] = g - s * (h + g * tau);\
  a[k * n + l] = h + s * (g - h * tau);\
} 


/**
 * Computes eigenvalues of a real summetric matrix
 */

template <typename FT> class Jacobi {
  public:
    Jacobi()
    {
      mMaxSweeps = 50;
      mFirstIters = 4;
      mAlpha = 100.;
    }
    ~Jacobi()
    {
    }
    
    void sort(int n, FT* d, FT* v)
    {
      int k, j, i;
      FT p;
      
      for(int i = 0; i < n -1; i ++) {
        p = d[i];
        k = i;
        for(j = i + 1; j < n; j ++){
          if(d[j] > p) {
            p = d[j];
            k = j;
          }
        }
        if(k != i) {
          d[k] = d[i];
          d[i] = p;
          for(j = 0; j < n; j ++) {
            p = v[j * n + i];
            v[j * n + i] = v[j * n + k];
            v[j * n + k] = p;
          }
        }
      }
    }
    
    void solve(int n, FT* a, FT* d, FT* v)
    {
      for(int i = 0; i < n; i ++)
        d[i] = a[i * n + i];
      for(int i = 0; i < n; i ++){
        for(int j = 0; j < n; j ++) {
            v[i * n + j] = (i == j) ? 1. : 0.;
        }
      }
      
      for(int s = 0; s < mMaxSweeps; s ++) {
        FT sm;
        sm = 0.;
        for(int i = 0; i < n  - 1; i ++)
          for(int j = i + 1; j < n; j ++)
          sm += BNBABS(a[i * n + j]);
        if(sm == 0.0)
          return;
        else
          sweep(n, /*false*/ s > mFirstIters,  0. /*(s < mFirstIters) ? 0.2 * sm /(n * n) : 0*/, a , d, v);
      }
    }
    
    void sweep(int n, bool skip, FT tresh, FT* a, FT* d, FT* v)
    {
      int nrot;
      nrot = 0;
      for(int ip = 0; ip < n - 1; ip ++) {
        for(int iq = ip + 1; iq < n; iq ++) {
          FT g;
          g = mAlpha * BNBABS(a[ip * n + iq]);
          if(skip) {
            if((BNBABS(d[ip] + g) == BNBABS(d[ip])) &&
                (BNBABS(d[iq] + g) == BNBABS(d[iq]))) {
              a[ip * n + iq] = 0.;
            }
          } else if(BNBABS(a[ip * n + iq]) > tresh) {
            FT h, t, c, s, tau;
            h = d[iq] - d[ip];
            if((FT)(BNBABS(h) + g) == (FT)BNBABS(h)) 
              t = a[ip * n + iq] / h;
            else {
              FT theta;
              theta = 0.5 * h / a[ip * n + iq];
              t = 1./(BNBABS(theta)+sqrt(1. + theta * theta));
              if(theta < 0.)
                t = -t;
            }            
            c = 1.0/sqrt(1. + t * t);
            s = t * c;
            tau = s / (1. + c);
            h = t * a[ip * n + iq];
            d[ip] -= h;
            d[iq] += h;
            a[ip * n + iq] = 0;
            for(int j = 0; j <= ip - 1; j ++) {
              ROTATE(a, j, ip, j, iq);
            }
            for(int j = ip + 1; j <= iq - 1; j ++) {
              ROTATE(a, ip, j, j, iq);
            }
            for(int j = iq + 1; j < n; j ++) {
              ROTATE(a, ip, j, iq, j);
            }
            for(int j = 0; j < n; j ++) {
              ROTATE(v, j, ip, j, iq);
            }
/*            for(int i = 0; i < n - 1; i ++)
              for(int j = i + 1; j < n; j ++)
                printf("%lf ", a[i * n + j]);
            printf("tau = %lf, s = %lf[%d, %d]\n", tau, s, ip + 1, iq + 1);*/
            nrot ++;
          }
        }
      }
    }
    
    /**
     * Solve system of linear equations using eigensystem 
     * @param n dimensionality
     * @param d eigenvalues
     * @param v nxn eigen vector matrix
     * @param c right part
     * @param x solution
     */
    void solveSys(int n, FT* d, FT* v, FT* c, FT* x)
    {
      static const int N = 1000;
      FT y1[N];
      FT *y2 = NULL;
      FT *y;
      
      if(n > N) {
        y2 = (FT*)malloc(n * sizeof(FT));
        y = y2;
      } else
        y = y1;
        
      for(int i = 0; i < n; i ++)
        y[i] = 0.;
      for(int i = 0; i < n; i ++) 
        for(int j = 0; j < n; j ++)
          y[j] += v[i * n + j] * c[i];
      
      for(int i = 0; i < n; i ++)
        y[i] /= d[i];
      
      for(int i = 0; i < n; i ++) {
        FT t;
        t = 0.;
        for(int j = 0; j < n; j ++)
          t += v[i * n + j] * y[j];
        x[i] = t;
      }
      if(y2)
        free(y2);
    }
    
  private:    
    int mMaxSweeps;
    int mFirstIters;
    double mAlpha;
};
#endif
