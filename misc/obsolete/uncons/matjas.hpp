#ifndef __MATJAS__
#define __MATJAS__

#include <problems/cover/common/intobj.hpp>
#include <util/common/interval.hpp>

/**
 * The optimization target for Matjas function
 */

template <typename FT> class Matjas : public IntervalObjective <FT> {
  public:
    
    FT func(const FT* x)
    {
      return  0.26 * (BNBSQR(x[0]) + BNBSQR(x[1])) - 0.48 * x[0] * x[1];
    }
    
    void grad(const FT* x, FT* g)
    {
    }
    
    void funcMin(FT* avec, FT* bvec, FT* minv)
    {
      FT a, b, c, d;
      
      BNBInterval<FT>::sqr(avec[0], bvec[0], &a, &b);
      BNBInterval<FT>::sqr(avec[1], bvec[1], &c, &d);
      BNBInterval<FT>::sum(a, b, c, d,  &a, &b);
      BNBInterval<FT>::mult(a, b, 0.26, 0.26, &a, &b);
      BNBInterval<FT>::mult(avec[0], bvec[0], avec[1], bvec[1], &c, &d);
      BNBInterval<FT>::mult(c, d, -0.48, -0.48, &c, &d);
      BNBInterval<FT>::sum(a, b, c, d, &c, &d);
      *minv = c;
    }
    
    void gradI(FT* avec, FT* bvec, FT* lbound, FT* ubound)
    {
      FT a, b, c, d;
      BNBInterval<FT>::mult(avec[0], bvec[0], 0.52, 0.52, &a, &b);
      BNBInterval<FT>::mult(avec[1], bvec[1], 0.48, 0.48, &c, &d);
      BNBInterval<FT>::diff(a, b, c, d, lbound, ubound);
      
      BNBInterval<FT>::mult(avec[1], bvec[1], 0.52, 0.52, &a, &b);
      BNBInterval<FT>::mult(avec[0], bvec[0], 0.48, 0.48, &c, &d);
      BNBInterval<FT>::diff(a, b, c, d, lbound + 1, ubound + 1);
    }
    
    virtual void form1(FT* avec, FT* bvec, FT* minv)
    {
      FT m[2], ll[2], lu[2], a, b, c, d, c1, c2;
      for(int i = 0; i < 2; i ++) {
        m[i] = 0.5 * (avec[i] + bvec[i]);
        ll[i] = 0.5 * (avec[i] - bvec[i]);
        lu[i] = 0.5 * (bvec[i] - avec[i]);
      }
      c1 = 0.52 * m[0] - 0.48 * m[1];
      c2 = 0.52 * m[1] - 0.48 * m[0];
      BNBInterval<FT>::mult(ll[0], lu[0], c1, c1, &a, &b);
      BNBInterval<FT>::mult(ll[1], lu[1], c2, c2, &c, &d);
      BNBInterval<FT>::sum(a, b, c, d, &a, &b);
      BNBInterval<FT>::mult(ll[0], lu[0], ll[1], lu[1], &c, &d);
      BNBInterval<FT>::mult(c, d, 0.96, 0.96, &c, &d);
      BNBInterval<FT>::diff(a, b, c, d, &c, &d);
      *minv = a;
    }
    
    void fillBounds(int dim, FT *a, FT* b)
    {
      for(int i = 0; i < dim; i ++) {
        a[i] = -10.;
        b[i] = 10.;
      }
    }


};
#endif
