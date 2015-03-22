#ifndef __POLIN_HPP__
#define __POLIN_HPP__

#include <problems/go/lipzobjective.hpp>
#include <util/common/interval.hpp>

class Polynom : public LipzObjective<double> {
  public:

    /**
     * Constructor
     * @param n polynom degree
     * @param coeff polynom coefficient from highest degree
     */
    Polynom(int n, double *coeff) {
      BNB_ASSERT(n >= 3);
      setDim(1);
      mN = n;
      mF = (double*)malloc((n + 1) * sizeof(double));
      mDF = (double*)malloc((n + 1) * sizeof(double));
      mDDF = (double*)malloc((n + 1) * sizeof(double));
      mDDDF = (double*)malloc((n + 1) * sizeof(double));
      for(int i = 0; i <= n; i ++) {
       mF[i] = coeff[i]; 
      }
      derive(n, mF, mDF);
      derive(n - 1, mDF, mDDF);
      derive(n - 2, mDDF, mDDDF);
      printf("\nDF: ");
      for(int i = 0; i <= (n - 1); i ++) 
        printf("%lf ", mDF[i]);
      printf("\nDDF: ");
      for(int i = 0; i <= (n - 2); i ++) 
        printf("%lf ", mDDF[i]);
      printf("\nDDDF: ");
      for(int i = 0; i <= (n - 3); i ++) 
        printf("%lf ", mDDDF[i]);

    }
    
    ~Polynom()
    {
      free(mF);
      free(mDF);
      free(mDDF);
      free(mDDDF);
    }
    
    double func(double* x)
    {
      double y = x[0];
      double v = getFunc(mN, mF, y) ;
      return v;
    }
        
    void grad(double* x, double* g)
    {
      double y = x[0];
      double v = getFunc(mN - 1, mDF, y) ;
      g[0] = v;
    }
    
    int hessn(double* x, double* H)
    {
      double y = x[0];
      double v = getFunc(mN - 2, mDDF, y) ;
      H[0] = v;
    }
    
    double getFuncLipzConstant(double* a, double* b)
    {
      double v, A = a[0], B = b[0], x, y;
      BNBInterval<double>::polynom(mN - 1, mDF, A, B, &x, &y);
      return BNBMAX(BNBABS(x), BNBABS(y));
    }
   
    double getGradLipzConstant(double* a, double* b)
    {
      double v, A = a[0], B = b[0], x, y;
      BNBInterval<double>::polynom(mN - 2, mDDF, A, B, &x, &y);
      return BNBMAX(BNBABS(x), BNBABS(y));
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      return getGradLipzConstant(a, b);
    }
    
    double getHessLipzConstant(double* a, double* b)
    {
      double v, A = a[0], B = b[0], x, y;
      BNBInterval<double>::polynom(mN - 3, mDDDF, A, B, &x, &y);
      return BNBMAX(BNBABS(x), BNBABS(y));
    }
    
    
    
  private:
    
    double getFunc(int n, double *fc, double x)
    {
      double v = 0.;
      for(int i = 0; i <= n; i ++) {
        v *= x;
        v += fc[i];        
      }      
      return v;
    }
    
    void derive(int n, double* fc, double* dfc)
    {
      for(int i = 0; i <= (n - 1); i ++) {
        dfc[i] = (n - i) * fc[i];
      }
    }
    int mN;
    double  *mF, *mDF, *mDDF, *mDDDF;
};

#endif
