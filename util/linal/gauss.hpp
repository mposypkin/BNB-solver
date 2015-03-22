#ifndef __GAUSS_HPP__
#define __GAUSS_HPP__

#include <stdlib.h>
#include <util/common/bnberrcheck.hpp>
#include <util/common/util.hpp>

/**
 * LU decomposition (Gaussian elemination)
 */
template <typename FT> class Gauss {
  public :

    /**
     * Solve a n x n system Ax = b
     * @param n dimensionality
     * @param A Ab
     * @param x result
     */
    bool solve(int n, FT* A, FT* x)
    {
      for(int i = 0; i < (n - 1); i ++) {
        int k;
        FT le;
        le = 0.;
        for(int j = i; j < n; j ++) {
          FT ce;
          ce = BNBABS(*(A + j * (n + 1) + i));
          if(ce > le) {
            k = j;
            le = ce;
          }
        } 
        if(le == 0)
          return false;
          //BNB_ERROR_REPORT("Unexpected zero element");
        if(k != i) {
          for(int l = i; l < n + 1; l ++) {
            FT tmp1, tmp2;
            tmp1 = *(A + i * (n + 1) + l);
            tmp2 = *(A + k * (n + 1) + l);
            *(A + k * (n + 1) + l) = tmp1;
            *(A + i * (n + 1) + l) = tmp2;
          }
        } 
        FT  ai;
        ai = *(A + i * (n + 1) + i);
        for(int l = i + 1; l < n; l ++) {
          FT al, r;
            
          al = *(A + l * (n + 1) + i);
          if(al != 0) {
            r = al / ai;
            *(A + l * (n + 1) + i) = 0;
            for(int m = i + 1; m < n + 1; m ++) {
              *(A + l * (n + 1) + m) -= *(A + i * (n + 1) + m) * r;
            }
          }
        }
      }
      for(int i = n -1; i >=0; i --) {
        FT aii;
        aii = *(A + i * (n + 1) + i);
        if(aii == 0)
          x[i] = 0;
        else {
          FT rp;
          
          rp = *(A + i * (n + 1) + n);
          for(int j = n - 1; j > i; j --) 
            rp -= (*(A + i * (n + 1) + j)) * x[j]; 
          x[i] = rp / aii;
        }
      }
      return true;
    }
    
  private:
    int n;
};
#endif
