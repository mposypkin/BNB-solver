#ifndef __POLI4_HPP__
#define __POLI4_HPP__

#include <problems/go/lipzobjective.hpp>
#include <util/common/interval.hpp>

class Poli4 : public LipzObjective<double> {
  public:
    
    Poli4 ()
    {
      setDim(1);
    }
    
    double func(double* x)
    {
      double y = x[0];
      double v = 3 * y * y * y * y -16 * y * y * y + 18 * y * y;
      return v;
    }
        
    void grad(double* x, double* g)
    {
      double y = x[0];
      g[0] = 12 * y * y * y - 48 * y * y + 36 * y;
    }
    
    int hessn(double* x, double* H)
    {
      double y = x[0];
      H[0] = 36 * y * y - 96 * y + 36;
      return 1;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double v, A = a[0], B = b[0], x, y, l, u;
      x = 0;
      y = 0;
      BNBInterval<double>::degree(3, A, B, &l, &u);
      BNBInterval<double>::sum(x, y, l, u, &x, &y);
      BNBInterval<double>::degree(2, A, B, &l, &u);
      BNBInterval<double>::diff(x, y, 4. * l, 4. * u, &x, &y);
      BNBInterval<double>::sum(x, y, 3. * A, 3. * B, &x, &y);
      v = BNBMAX(BNBABS(x), BNBABS(y));
      return 12. * v;
    }
   
    double getGradLipzConstant(double* a, double* b)
    {
      double v, A = a[0], B = b[0], x, y, l, u;
      x = 0;
      y = 0;
      BNBInterval<double>::degree(2, A, B, &l, &u);
      BNBInterval<double>::sum(x, y, 3. * l, 3. * u, &x, &y);
      BNBInterval<double>::diff(x, y, 8. * A, 8. * B, &x, &y);
      BNBInterval<double>::sum(x, y, 3., 3., &x, &y);
      v = BNBMAX(BNBABS(x), BNBABS(y));
      return 12. * v;
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      return getGradLipzConstant(a, b);
    }
    
    double getHessLipzConstant(double* a, double* b)
    {
      double v, A = a[0], B = b[0], x, y, l, u;
      BNBInterval<double>::diff(72. * A, 72. * B, 96., 96., &x, &y);
      v = BNBMAX(BNBABS(x), BNBABS(y));
      return v;
    }
    
};
#endif
