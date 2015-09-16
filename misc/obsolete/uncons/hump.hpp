#ifndef __HUMP__
#define __HUMP__

#include <math.h>
#include <problems/cover/common/intobj.hpp>
#include <util/common/interval.hpp>


/**
 * The optimization target for Hump function
 */

template <typename FT> class Hump : public IntervalObjective <FT> {
  public:
    
    FT func(const FT* x)
    {
      FT rv;
      rv =   4. * x[0] * x[0] - 2.1 * x[0] * x[0] * x[0] * x[0] + 
          (1./3.) * x[0] * x[0] * x[0] * x[0] * x[0] * x[0] + x[0] * x[1]  
          - 4. * x[1] * x[1] + 4. * x[1] * x[1] * x[1] * x[1];
      return rv;
    }
    
    void grad(const FT* x, FT* g)
    {
    }
    
    void funcMin(FT* avec, FT* bvec, FT* minv)
    {
      FT a, b, c, d, e, f, C;
      
      BNBInterval<FT>::sqr(avec[0], bvec[0], &a, &b);
      BNBInterval<FT>::mult(a, b, 4., 4., &c, &d);
      BNBInterval<FT>::sqr(a, b, &e, &f);
      BNBInterval<FT>::diff(c, d, 2.1 * e, 2.1 * f, &c, &d);
      BNBInterval<FT>::mult(a, b, e, f, &e, &f);
      C = 1./3.;
      BNBInterval<FT>::sum(c, d, C * e, C * f, &c, &d);
      BNBInterval<FT>::mult(avec[0], bvec[0], avec[1], bvec[1], &a, &b);
      BNBInterval<FT>::sum(a, b, c, d, &c, &d);
      BNBInterval<FT>::sqr(avec[1], bvec[1], &a, &b);
      BNBInterval<FT>::diff(c, d, 4. * a, 4. * b, &c, &d);
      BNBInterval<FT>::sqr(a, b, &a, &b);
      BNBInterval<FT>::sum(c, d, 4. * a, 4. * b, &c, &d);
      *minv = c;
    }
    
    void gradI(FT* avec, FT* bvec, FT* lbound, FT* ubound)
    {
      FT a0, b0, a, b, c, d, C;
      BNBInterval<FT>::mult(8., 8., avec[0], bvec[0], &a, &b);
      BNBInterval<FT>::degree(3, avec[0], bvec[0], &c, &d);
      BNBInterval<FT>::diff(a, b, 8.4 * c, 8.4 * d, &a, &b);
      BNBInterval<FT>::degree(5, avec[0], bvec[0], &c, &d);
      BNBInterval<FT>::sum(a, b, 2. * c, 2. * d, &a, &b);
      BNBInterval<FT>::sum(a, b, avec[1], bvec[1], lbound, ubound);
      
      BNBInterval<FT>::diff(avec[0], bvec[0], 8. * avec[1], 8. * bvec[1], &a, &b);
      BNBInterval<FT>::degree(3, avec[1], bvec[1], &c, &d);
      BNBInterval<FT>::sum(a, b, 16. * c, 16. * d, lbound + 1, ubound + 1);
    }
    
    virtual void form1(FT* avec, FT* bvec, FT* minv)
    {
    }
    
    void fillBounds(int dim, FT *a, FT* b)
    {
      a[0] = -5.;
      b[0] = 5.;
      a[1] = -5.;
      b[1] = 5.;
    }


};
#endif
