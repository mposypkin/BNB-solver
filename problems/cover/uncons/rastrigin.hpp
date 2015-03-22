#ifndef __RASTRIGIN__ 
#define __RASTRIGIN__


#include <problems/cover/common/intobj.hpp>
#include <util/common/interval.hpp>

/**
 * The optimization target for Rastrigin function
 */

template <typename FT> class Rastrigin : public IntervalObjective <FT> {
  public:
    
    FT func(const FT* x)
    {
      return  20 + BNBSQR(x[0]) + BNBSQR(x[1]) - 10. * cos(2 * M_PI * x[0]) - 10. * cos(2 * M_PI * x[1]);
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
      BNBInterval<FT>::sum(a, b, 20, 20,  &a, &b);
      BNBInterval<FT>::mult(avec[0], bvec[0], 2. * M_PI, 2. * M_PI, &c, &d);
      BNBInterval<FT>::cos(c, d, &c, &d);
      BNBInterval<FT>::mult(c, d, 10., 10., &c, &d);
      BNBInterval<FT>::diff(a, b, c, d,  &a, &b);
      BNBInterval<FT>::mult(avec[1], bvec[1], 2. * M_PI, 2. * M_PI, &c, &d);
      BNBInterval<FT>::cos(c, d, &c, &d);
      BNBInterval<FT>::mult(c, d, 10., 10., &c, &d);
      BNBInterval<FT>::diff(a, b, c, d,  &a, &b);
      *minv = a;
    }
    
    void gradI(FT* avec, FT* bvec, FT* lbound, FT* ubound)
    {
      int n;
      
      n = IntervalObjective <FT>::getDim();
      for(int i = 0; i < n; i ++) {
        FT a, b;
        BNBInterval<FT>::mult(avec[i], bvec[i], 2. * M_PI, 2. * M_PI, &a, &b);
        BNBInterval<FT>::sin(a, b, &a, &b);
        BNBInterval<FT>::mult(a, b, 20. * M_PI, 20. * M_PI, &a, &b);
        BNBInterval<FT>::sum(2 * avec[i], 2 * bvec[i], a, b, lbound + i, ubound + i);        
      }        
    }
    
    virtual void form1(FT* avec, FT* bvec, FT* minv)
    {
    }
    
    void fillBounds(int dim, FT *a, FT* b)
    {
      for(int i = 0; i < dim; i ++) {
        a[i] = -5.12;
        b[i] = 5.12;
      }
    }


};
#endif
