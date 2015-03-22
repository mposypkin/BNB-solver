#ifndef __BRANIN__
#define __BRANIN__

#include <math.h>
#include <problems/cover/common/intobj.hpp>
#include <util/common/interval.hpp>


/**
 * The optimization target for Branin function
 */

template <typename FT> class Branin : public IntervalObjective <FT> {
  public:
    
    FT func(const FT* x)
    {
      return  BNBSQR(x[1] - (5./(4. * BNBSQR(M_PI))) * BNBSQR(x[0]) + (5./M_PI) * x[0] - 6.) +
          10.*(1.-(1./(8. * M_PI)))*cos(x[0]) + 10.;
    }
    
    void grad(const FT* x, FT* g)
    {
    }
    
    void funcMin(FT* avec, FT* bvec, FT* minv)
    {
      FT a, b, c, d, C;
      
      BNBInterval<FT>::sqr(avec[0], bvec[0], &a, &b);
      C = 5. / (4. * BNBSQR(M_PI));
      BNBInterval<FT>::mult(a, b, C, C, &a, &b);
      BNBInterval<FT>::diff(avec[1], bvec[1], a, b, &a, &b);
      C = 5. / M_PI;
      BNBInterval<FT>::mult(avec[0], bvec[0], C, C, &c, &d);
      BNBInterval<FT>::sum(a, b, c, d, &a, &b);
      BNBInterval<FT>::diff(a, b, 6., 6., &a, &b);
      BNBInterval<FT>::sqr(a, b, &a, &b);
      BNBInterval<FT>::cos(avec[0], bvec[0], &c, &d);
      C = 10. * (1. - 1./(8. * M_PI));
      BNBInterval<FT>::mult(c, d, C, C, &c, &d);
      BNBInterval<FT>::sum(a, b, c, d, &c, &d);
      BNBInterval<FT>::sum(c, d, 10., 10., &c, &d);          
      *minv = c;
    }
    
    void gradI(FT* avec, FT* bvec, FT* lbound, FT* ubound)
    {
      FT a0, b0, a, b, c, d, C;
      
      BNBInterval<FT>::sqr(avec[0], bvec[0], &a, &b);
      C = 5. / (4. * BNBSQR(M_PI));
      BNBInterval<FT>::mult(a, b, C, C, &a, &b);
      BNBInterval<FT>::diff(avec[1], bvec[1], a, b, &a, &b);
      C = 5. / M_PI;
      BNBInterval<FT>::mult(avec[0], bvec[0], C, C, &c, &d);
      BNBInterval<FT>::sum(a, b, c, d, &a, &b);
      BNBInterval<FT>::diff(a, b, 6., 6., &a, &b);
      BNBInterval<FT>::mult(2., 2., a, b, &a, &b);
      lbound[1] = a;
      ubound[1] = b;
      C = 5./(2. * BNBSQR(M_PI));
      BNBInterval<FT>::mult(avec[0], bvec[0], C, C, &c, &d);
      C = 5./M_PI;
      BNBInterval<FT>::diff(C, C, c, d, &c, &d);
      BNBInterval<FT>::mult(a, b, c, d, &a, &b);
      BNBInterval<FT>::sin(avec[0], bvec[0], &c, &d);
      C = 10. * (1. - 1./(8. * M_PI));
      BNBInterval<FT>::mult(c, d, C, C, &c, &d);
      BNBInterval<FT>::diff(a, b, c, d, lbound, ubound);
    }
    
    virtual void form1(FT* avec, FT* bvec, FT* minv)
    {
    }
    
    void fillBounds(int dim, FT *a, FT* b)
    {
      a[0] = -5.;
      b[0] = 10.;
      a[1] = 0.;
      b[1] = 15.;
    }


};
#endif
