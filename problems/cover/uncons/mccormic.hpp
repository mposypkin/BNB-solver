#ifndef __MCCORMIC__
#define __MCCORMIC__

#include <problems/cover/common/intobj.hpp>
#include <util/common/interval.hpp>

/**
 * The optimization target for McCormic function
 */

template <typename FT> class McCormic : public IntervalObjective <FT> {
  public:
    
    FT func(const FT* x)
    {
      return  sin(x[0] + x[1]) + BNBSQR(x[0] - x[1]) - 1.5 * x[0] + 2.5 * x[1] + 1;
    }
    
    void grad(const FT* x, FT* g)
    {
    }
    
    void funcMin(FT* avec, FT* bvec, FT* minv)
    {
      FT a, b, c, d;
      
      BNBInterval<FT>::sum(avec[0], bvec[0], avec[1], bvec[1],  &a, &b);
      BNBInterval<FT>::sin(a, b, &a, &b);
      BNBInterval<FT>::diff(avec[0], bvec[0], avec[1], bvec[1],  &c, &d);
      BNBInterval<FT>::sqr(c, d, &c, &d);
      BNBInterval<FT>::sum(a, b, c, d, &c, &d);
      BNBInterval<FT>::mult(avec[0], bvec[0], -1.5, -1.5, &a, &b);
      BNBInterval<FT>::sum(a, b, c, d, &c, &d);
      BNBInterval<FT>::mult(avec[1], bvec[1], 2.5, 2.5, &a, &b);
      BNBInterval<FT>::sum(a, b, c, d, &c, &d);
      BNBInterval<FT>::sum(1., 1., c, d, &c, &d);
      *minv = c;
    }
    
    void gradI(FT* avec, FT* bvec, FT* lbound, FT* ubound)
    {
      FT a, b, cosl, cosu;
      BNBInterval<FT>::sum(avec[0], bvec[0], avec[1], bvec[1],  &a, &b);
      BNBInterval<FT>::cos(a, b, &cosl, &cosu);
      BNBInterval<FT>::diff(avec[0], bvec[0], avec[1], bvec[1],  &a, &b);
      BNBInterval<FT>::mult(a, b, 2., 2., &a, &b);
      BNBInterval<FT>::sum(cosl, cosu, a, b, &a, &b);
      BNBInterval<FT>::sum(a, b, -1.5, -1.5, lbound, ubound);
      BNBInterval<FT>::diff(avec[1], bvec[1], avec[0], bvec[0],  &a, &b);
      BNBInterval<FT>::mult(a, b, 2., 2., &a, &b);
      BNBInterval<FT>::sum(cosl, cosu, a, b, &a, &b);
      BNBInterval<FT>::sum(a, b, 2.5, 2.5, lbound + 1, ubound + 1);
    }
    
    virtual void form1(FT* avec, FT* bvec, FT* minv)
    {
      FT m[2], ll[2], lu[2], a, b, c, d, sl, su,cosl, cosu;
      for(int i = 0; i < 2; i ++) {
        m[i] = 0.5 * (avec[i] + bvec[i]);
        ll[i] = 0.5 * (avec[i] - bvec[i]);
        lu[i] = 0.5 * (bvec[i] - avec[i]);
      }
      BNBInterval<FT>::sum(avec[0], bvec[0], avec[1], bvec[1],  &a, &b);
      BNBInterval<FT>::cos(a, b, &cosl, &cosu);
      a = cosl + 2. * (m[0] - m[1]) - 1.5;
      b = cosu + 2. * (m[0] - m[1]) - 1.5;
      c = cosl + 2. * (m[1] - m[0]) + 2.5;
      d = cosu + 2. * (m[1] - m[0]) + 2.5;
      BNBInterval<FT>::mult(a, b, ll[0], lu[0], &a, &b);
      BNBInterval<FT>::mult(c, d, ll[1], lu[1], &c, &d);
      BNBInterval<FT>::sum(a, b, c, d, &c, &d);
      *minv = c;
    }
    
    void fillBounds(int dim, FT *a, FT* b)
    {
      a[0] = -1.5;
      b[0] = 4.;
      a[1] = -3.;
      b[1] = 4.;
    } 
};
#endif
