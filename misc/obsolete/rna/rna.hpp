#ifndef __RNA_HPP__
#define __RNA_HPP__

#include <sstream>
#include <map>

#include <problems/go/lipzobjective.hpp>
#include <util/common/bnberrcheck.hpp>
#include <util/common/vec.hpp>
#include <util/common/matr.hpp>
#include "rnabase.hpp"
#include <util/common/interval.hpp>
#include <util/common/util.hpp>
#include <util/common/smartptr.hpp>

class RNA : public LipzObjective <double> {
  public:
    
    typedef Objective<double> BASE;
    
    RNA() : Objective <double>()
    {
      mRadius = 1.;
      mRNABase = NULL;      
    }
    
    void setRadius(double rad)
    {
      mRadius = rad;
    }
    
    void setRNABase(RNABase<double>* base)
    {
      mRNABase = base;
      setDim(base->getDim());
      mAux.alloc(base->getDim());
    }
    
    RNABase<double>* getRNABase() const
    {
      return mRNABase;
    }
    
    /**
     * Forms a string representation for DNA
     * @return string represenatiton
     */
    std::string toString() 
    {
      std::ostringstream os;
      /*
      int n = BASE::getDim();
      VecUtils::vecPrint(n - 1, mSDiag);
      os << "kappa: ";
      for(int i = 0;i < n; i ++) {
        os << mKap[i];
        if(i != (n - 1))
          os << ", ";
      }
      os << "\nmain diag: ";
      for(int i = 0; i < n; i ++) {
        os << mDiag[i];
        if(i != (n - 1)) 
          os << ", ";
      }
      os << "\nsecond diag: ";
      for(int i = 0; i < (n - 1); i ++) {
        os << mSDiag[i];
        os << ", ";
      }
      */
      return os.str();      
    }
    /**
     * The objective function itself (projected)
     *
     * @param x argument
     *
     * @return function value
     *
     */
    double func(const double* x)
    {
      int n;
      double v;
      
      projectPoint(x, mAux);
      v = funcBase(mAux);
      return v;
    }
    
    /**
     * The objective function itself 
     *
     * @param x argument
     *
     * @return function value
     *
     */
    double funcBase(double* x)
    {
      BNB_ASSERT(mRNABase);
      return mRNABase->func(x);
    }
    
  /**
   * Gradient (projected)
   * 
   * @param x argument
   * @param g gradient
   */
    void grad(const double* x, double* g)
    {
      projectPoint(x, mAux);
      gradBase(mAux, g);
      projectVector(x, g);
    }
  
    /**
     * Gradient 
     * 
     * @param x argument
     * @param g gradient
     */
    void gradBase(double* x, double* g)
    {
      BNB_ASSERT(mRNABase);
      mRNABase->grad(x, g);
    }
    
    /**
     * Calculate Hessian matrix
     * @param x point
     * @param H hessian 
     * @return dimension (may be less then n with possible constraints)
     */
    int hessn(const double* X, double* H)
    {
      class Sparse {
        public :
          typedef std::map<unsigned int, double> Vec;
          static double mult(Vec& u, Vec& v)
          {
            double s = 0.;
            Vec::const_iterator it = u.begin();
            for(;it != u.end(); it ++) {
              const unsigned int i = it->first;
              if(v.find(i) != v.end()) {
                double t = it->second;
                s += t * v[i];
              }
            }
            return s;  
          }
          
          static void print(const Vec& u)
          {
            Vec::const_iterator it = u.begin();
            for(;it != u.end(); it ++) {
              printf("v[%d] = %lf\n", it->first, it->second);
            }
          }
      };
      int n = BASE::getDim();
      int m = n - 1;
      int k = 0;
      double t = BNBABS(X[k]);
      double *diag, *sdiag, *kap;
      BNB_ASSERT(mRNABase);
      kap = mRNABase->getKap();
      diag = mRNABase->getDiag();
      sdiag = mRNABase->getSDiag();
      Sparse::Vec wk;
      projectPoint(X, mAux);
      double* x = mAux;
      for(int i = 0; i < n; i ++) {
        if(BNBABS(x[i]) > t) {
          k = i;
          t = BNBABS(x[k]);
        }                    
      }
      t =  diag[k] * x[k] - kap[k] * x[k] * x[k] * x[k];
      if(k != (n - 1)) {
        t += sdiag[k] * x[k + 1];
      }
      if(k != 0) {
        t += sdiag[k - 1] * x[k - 1];
      }
      double lam = t / x[k];

      wk[k] = 2. * diag[k] - 6. * kap[k] * x[k] * x[k] - 2. * lam;
      if(k != 0)
        wk[k - 1] = 2. * sdiag[k - 1];
      if(k != (n - 1))
        wk[k + 1] = 2. * sdiag[k];
      for(int i = 0; i < m; i ++) {
        for(int j = 0; j < m; j ++) {
          double s = 0.;
          Sparse::Vec u, v, wI, z;
          int I, J;
          I = (i < k) ? i : i + 1;
          J = (j < k) ? j : j + 1;
          u[I] = 1.;
          u[k] = -x[I] / x[k];            
          v[J] = 1.;
          v[k] = -x[J] / x[k];    
          wI[I] = 2. * diag[I] - 6. * kap[I] * x[I] * x[I] - 2. * lam;
          if(I != 0)
            wI[I - 1] = 2. * sdiag[I - 1];
          if(I != (n - 1))
            wI[I + 1] = 2. * sdiag[I];
          z[k] = Sparse::mult(wk, v);
          z[I] = Sparse::mult(wI, v);
          s = Sparse::mult(u, z);
          H[i * m + j] = s;
        }
      }
      return m;
    }
    
    
    double getFuncLipzConstant(double* A, double* B)
    {
      double* kap = mRNABase->getKap();
      double* diag = mRNABase->getDiag();
      double* sdiag = mRNABase->getSDiag();      
      int n = BASE::getDim();
      double v = 0.;
      for(int i = 0; i < n; i ++) {
        double xa, xb, a, b, ga, gb, h;
        xa = A[i];
        xb = B[i];
        BNBInterval<double>::degree(3, xa, xb, &a, &b);
        BNBInterval<double>::mult(a, b, -kap[i], -kap[i], &ga, &gb);      
        h = diag[i];
        BNBInterval<double>::mult(xa, xb, h, h, &a, &b);
        BNBInterval<double>::sum(ga, gb, a, b, &ga, &gb);  
        if(i != 0) {
          xa = A[i - 1];
          xb = B[i - 1];
          h = sdiag[i - 1];
          BNBInterval<double>::mult(xa, xb, h, h  , &a, &b);
          BNBInterval<double>::sum(ga, gb, a, b, &ga, &gb);
        }
        if(i != (n - 1)) {
          xa = A[i + 1];
          xb = B[i + 1];
          h = sdiag[i];
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
      int n = BASE::getDim();     
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
      int n = BASE::getDim();
      
      q = BNBSQR(getGIJ(i, i, a, b));
      if(i != 0) {
        q += BNBSQR(getGIJ(i, i - 1, a, b));
      } else if(i != (n - 1)) {
        q += BNBSQR(getGIJ(i, i + 1, a, b));
      }
      rv = sqrt(q);
      return rv;
    }

  private:
    
    void projectPoint(const double* x, double *y)
    {
      int n;
      double r;
      double factor;
      
      n = BASE::getDim();
      
      r = VecUtils::vecNormTwo<double>(n, x);
      factor = mRadius / r;
      VecUtils::vecMult<double>(n, x, factor, y);
    }
    
    void projectVector(const double* x, double* v)
    {
      int n;
      double s;
      double factor;
      
      n = BASE::getDim();
      s = VecUtils::vecScalarMult(n, x, v);
      factor = s / (mRadius * mRadius);
      for(int i = 0; i < n; i ++) {
        v[i] -= x[i] * factor;
      }
    }
    
    double getGIJ(int i, int j, double* a, double* b)
    {
      double rv, xa, xb, nu, kap;
      if(j == i) {
        double A, B, C;
        xa = a[i];
        xb = b[i];
        nu = mRNABase->getDiag()[i];
        kap = mRNABase->getKap()[i];        
        A = BNBABS(nu  - 3 * kap * xa * xa);
        B = BNBABS(nu  - 3 * kap * xb * xb);
        C = BNBMAX(A, B);
        if((xa < 0) && (xb > 0)) 
          rv = BNBMAX(BNBABS(nu),C);
        else 
          rv = C;
      } else if(j == (i - 1)) {
        rv = BNBABS(mRNABase->getSDiag()[i - 1]);
      } else if(j == (i + 1)) {
        rv = BNBABS(mRNABase->getSDiag()[i]);
      } else {
        rv = 0.;
      }
      rv *= 2.; 
      return rv;
    }
    
    double  mRadius;
    RNABase<double> *mRNABase;
    SmartArrayPtr<double> mAux;
};

#endif
