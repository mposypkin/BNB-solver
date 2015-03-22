#ifndef __SHUBERT__
#define __SHUBERT__

#include <math.h>
#include <problems/cover/common/intobj.hpp>
#include <util/common/interval.hpp>


/**
 * The optimization target for Shubert function
 */

template <typename FT> class Shubert : public IntervalObjective <FT> {
  public:
    
    FT func(const FT* x)
    {
      FT a = 0., b = 0.;
      for(int i = 1; i <= 5; i ++) {
        a += i * cos((i + 1)*x[0] + i);
        b += i * cos((i + 1)*x[1] + i);
      }
      return  a * b;
    }
    
    void grad(const FT* x, FT* g)
    {
    }
    
    void funcMin(FT* avec, FT* bvec, FT* minv)
    {
      FT a, b, c, d, a0 = 0, b0 = 0, a1 = 0, b1 = 0;
      
      for(int i = 1; i <= 5; i ++) {
        BNBInterval<FT>::mult(i + 1, i + 1, avec[0], bvec[0], &a, &b);
        BNBInterval<FT>::sum(a, b, i, i, &a, &b);
        BNBInterval<FT>::cos(a, b, &a, &b);
        BNBInterval<FT>::mult(a, b, i, i, &a, &b);
        BNBInterval<FT>::sum(a0, b0, a, b, &a0, &b0);
      }
      for(int i = 1; i <= 5; i ++) {
        BNBInterval<FT>::mult(i + 1, i + 1, avec[1], bvec[1], &a, &b);
        BNBInterval<FT>::sum(a, b, i, i, &a, &b);
        BNBInterval<FT>::cos(a, b, &a, &b);
        BNBInterval<FT>::mult(a, b, i, i, &a, &b);
        BNBInterval<FT>::sum(a1, b1, a, b, &a1, &b1);
      }
      BNBInterval<FT>::mult(a0, b0, a1, b1, &c, &d);
      *minv = c;
    }
    
    void gradI(FT* avec, FT* bvec, FT* lbound, FT* ubound)
    {
      FT a, b, c, d, a0 = 0, b0 = 0, a1 = 0, b1 = 0;
      for(int i = 1; i <= 5; i ++) {
        BNBInterval<FT>::mult(i + 1, i + 1, avec[1], bvec[1], &a, &b);
        BNBInterval<FT>::sum(a, b, i, i, &a, &b);
        BNBInterval<FT>::cos(a, b, &a, &b);
        BNBInterval<FT>::mult(a, b, i, i, &a, &b);
        BNBInterval<FT>::sum(a0, b0, a, b, &a0, &b0);
      }
      for(int i = 1; i <= 5; i ++) {
        BNBInterval<FT>::mult(i + 1, i + 1, avec[0], bvec[0], &a, &b);
        BNBInterval<FT>::sum(a, b, i, i, &a, &b);
        BNBInterval<FT>::sin(a, b, &a, &b);
        BNBInterval<FT>::mult(a, b, i * (i + 1), i * (i + 1), &a, &b);
        BNBInterval<FT>::sum(a1, b1, a, b, &a1, &b1);
      }
      BNBInterval<FT>::mult(a0, b0, a1, b1, &c, &d);
      BNBInterval<FT>::mult(c, d, -1., -1., lbound, ubound);
      a0 = 0, b0 = 0, a1 = 0, b1 = 0;
      for(int i = 1; i <= 5; i ++) {
        BNBInterval<FT>::mult(i + 1, i + 1, avec[0], bvec[0], &a, &b);
        BNBInterval<FT>::sum(a, b, i, i, &a, &b);
        BNBInterval<FT>::cos(a, b, &a, &b);
        BNBInterval<FT>::mult(a, b, i, i, &a, &b);
        BNBInterval<FT>::sum(a0, b0, a, b, &a0, &b0);
      }
      for(int i = 1; i <= 5; i ++) {
        BNBInterval<FT>::mult(i + 1, i + 1, avec[1], bvec[1], &a, &b);
        BNBInterval<FT>::sum(a, b, i, i, &a, &b);
        BNBInterval<FT>::sin(a, b, &a, &b);
        BNBInterval<FT>::mult(a, b, i * (i + 1), i * (i + 1), &a, &b);
        BNBInterval<FT>::sum(a1, b1, a, b, &a1, &b1);
      }
      BNBInterval<FT>::mult(a0, b0, a1, b1, &c, &d);
      BNBInterval<FT>::mult(c, d, -1., -1., lbound + 1, ubound + 1);
    }
    
    virtual void form1(FT* avec, FT* bvec, FT* minv)
    {
    }
    
    void fillBounds(int dim, FT *a, FT* b)
    {
      a[0] = -10.;
      b[0] = 10.;
      a[1] = -10.;
      b[1] = 10.;
    }


};
#endif
