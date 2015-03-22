#ifndef __DNA_HPP__
#define __DNA_HPP__

#include <util/common/util.hpp>
#include <util/common/interval.hpp>
#include <problems/cover/common/lipzobjective.hpp>


class DNA : public LipzObjective<double> {
  public:
    
    DNA(int n, int lagm)
    {
      setDim(n);
      mNLagrangeMult = lagm;
    }
    
    double func(const double* x)
    {
      double v;
      int n = getDim() - mNLagrangeMult;
      
      v = 0.;
      for(int i = 0; i < n; i ++) {
        double y = mDiag[i] * x[i];
        y *= x[i];
        v += y;
        y = 0.5 * mKap[i];
        y *= x[i];
        y *= x[i];
        y *= x[i];
        y *= x[i];
        v -= y;
        if(i != (n - 1)) {
          y = mSDiag[i] * x[i];
          v +=  y * x[i + 1];
        }
        if(i != 0) {
          y = mSDiag[i-1] * x[i - 1];
          v += y * x[i];
        }
      }      
      return v;
    }
        
    void grad(const double* x, double* g)
    {
      int n = getDim() - mNLagrangeMult;
      for(int i = 0; i < n; i ++) {
        double v, y;
        v = mDiag[i] * x[i];
        y =  mKap[i] * x[i];
        y *= x[i];
        y *= x[i];
        v -=  y;
        if(i != (n - 1)) {
          v += mSDiag[i] * x[i + 1];
        }
        if(i != 0) {
          v += mSDiag[i - 1] * x[i - 1];
        }
        g[i] =  2. * v;
      }
      if(mNLagrangeMult == 1)
        g[n] = 0.;
    }
    
    double getFuncLipzConstant(double* A, double* B)
    {
      int n = getDim() - mNLagrangeMult;
      double v = 0.;
      for(int i = 0; i < n; i ++) {
        double xa, xb, a, b, ga, gb, h;
        xa = A[i];
        xb = B[i];
        BNBInterval<double>::degree(3, xa, xb, &a, &b);
        BNBInterval<double>::mult(a, b, -mKap[i], -mKap[i], &ga, &gb);      
        h = mDiag[i];
        BNBInterval<double>::mult(xa, xb, h, h, &a, &b);
        BNBInterval<double>::sum(ga, gb, a, b, &ga, &gb);  
        if(i != 0) {
          xa = A[i - 1];
          xb = B[i - 1];
          h = mSDiag[i - 1];
          BNBInterval<double>::mult(xa, xb, h, h  , &a, &b);
          BNBInterval<double>::sum(ga, gb, a, b, &ga, &gb);
        }
        if(i != (n - 1)) {
          xa = A[i + 1];
          xb = B[i + 1];
          h = mSDiag[i];
          BNBInterval<double>::mult(xa, xb, h, h, &a, &b);
          BNBInterval<double>::sum(ga, gb, a, b, &ga, &gb);
        }
        v += BNBMAX(4. * ga * ga, 4. * gb * gb);
      }
      return sqrt(v);    
    }
    
    double getGradLipzConstant(double* a, double* b)
    {
      double rv = 0.;
      int n = getDim()- mNLagrangeMult;    
      for(int i = 0; i < n; i ++) {
        double v = getGIJ(i, i, a, b);
        if(i != 0) {
          v += getGIJ(i, i - 1, a, b);
        } else if(i != (n - 1)) {
          v += getGIJ(i, i + 1, a, b);
        }
        if(v > rv)
          rv = v;
      }
      return rv;      
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      double rv, q = 0.;
      int n = getDim() - mNLagrangeMult;
      
      q = BNBSQR(getGIJ(i, i, a, b));
      if(i != 0) {
        q += BNBSQR(getGIJ(i, i - 1, a, b));
      } else if(i != (n - 1)) {
        q += BNBSQR(getGIJ(i, i + 1, a, b));
      }        
      rv = sqrt(q);
      return rv;
    }
    
    SmartArrayPtr<double>* getKap() 
    {
      return &mKap;
    }
    
    SmartArrayPtr<double>* getDiag()
    {
      return &mDiag;
    }
    
    SmartArrayPtr<double>* getSDiag()
    {
      return &mSDiag;
    }
    
  private:
    double getGIJ(int i, int j, double* a, double* b)
    {
      double rv, xa, xb, nu, kap;
      int n = getDim() - mNLagrangeMult;
      if(j == i) {
        double A, B, C;
        xa = a[i];
        xb = b[i];
        nu = (*getDiag())[i];
        kap = (*getKap())[i];        
        A = BNBABS(nu  - 3 * kap * xa * xa);
        B = BNBABS(nu  - 3 * kap * xb * xb);
        C = BNBMAX(A, B);
        if((xa < 0) && (xb > 0)) 
          rv = BNBMAX(BNBABS(nu),C);
        else 
          rv = C;
      } else if(j == (i - 1)) {
        rv = BNBABS((*getSDiag())[i - 1]);
      } else if(j == (i + 1)) {
        rv = BNBABS((*getSDiag())[i]);
      } 
      else {
        rv = 0.;
      }
      rv *= 2.; 
      return rv;
    }

    double mRadius;
    int mNLagrangeMult;
    SmartArrayPtr<double> mKap;
    SmartArrayPtr<double> mDiag;
    SmartArrayPtr<double> mSDiag;    
};

#endif
