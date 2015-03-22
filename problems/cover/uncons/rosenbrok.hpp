#ifndef __ROSENBROK_HPP_
#define __ROSENBROK_HPP_

#include <problems/cover/common/intobj.hpp>
#include <util/common/interval.hpp>

#define A 100
/**
 * The optimization target for Rosenbrok function
 */

template <typename FT> class Rosenbrok : public IntervalObjective <FT> {
  public:
    
    FT func(const FT* x)
    {
      return A * BNBSQR(x[1] - BNBSQR(x[0])) + BNBSQR(1. - x[0]);
    }
    
    void grad(const FT* x, FT* g)
    {
    }
    
    void funcMin(FT* avec, FT* bvec, FT* minv)
    {
      FT a, b, c, d;
      
      BNBInterval<FT>::sqr(avec[0], bvec[0], &a, &b);
      BNBInterval<FT>::diff(avec[1], bvec[1], a, b, &a, &b);
      BNBInterval<FT>::sqr(a, b, &a, &b);
      a *= A;
      b *= A;
      BNBInterval<FT>::diff(1., 1., avec[0], bvec[0], &c, &d);
      BNBInterval<FT>::sqr(c, d, &c, &d);
      BNBInterval<FT>::sum(a, b, c, d, &c, &d);
      *minv = c;
    }
    
    void gradI(FT* avec, FT* bvec, FT* lbound, FT* ubound)
    {
      FT a, b, c, d;
      BNBInterval<FT>::sqr(avec[0], bvec[0], &a, &b);
      BNBInterval<FT>::diff(a, b, avec[1], bvec[1], &a, &b);
      BNBInterval<FT>::mult(avec[0], bvec[0], a, b, &a, &b);
      BNBInterval<FT>::mult(4*A, 4*A, a, b, &a, &b);
      BNBInterval<FT>::diff(avec[0], bvec[0], 1, 1, &c, &d);
      BNBInterval<FT>::mult(2, 2, c, d, &c, &d);      
      BNBInterval<FT>::sum(a, b, c, d, &c, &d);
      lbound[0] = c;
      ubound[0] = d;
      BNBInterval<FT>::sqr(avec[0], bvec[0], &a, &b);
      BNBInterval<FT>::diff(avec[1], bvec[1], a, b, &a, &b);
      BNBInterval<FT>::mult(200., 200., a, b, &a, &b);
      lbound[1] = a;
      ubound[1] = b;
    }
    
    void fillBounds(int dim, FT *a, FT* b)
    {
      for(int i = 0; i < dim; i ++) {
        a[i] = -5.;
        b[i] = 10.;
      }
    }
};
#endif
