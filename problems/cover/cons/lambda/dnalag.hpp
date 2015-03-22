#ifndef __DNALAG_HPP__
#define __DNALAG_HPP__

#include <util/common/util.hpp>
#include <problems/cover/common/lipzobjective.hpp>
#include "dna.hpp"

class DNALag : public LipzObjective<double> {
  public:
    
    DNALag(DNA* dna)
    {
      mDNA = dna;
      setDim(mDNA->getDim());      
      mNLagrangeMult = 1;
    }
    
    double func(const double* x)
    {
      return 0.;
    }
        
    void grad(const double* x, double* g)
    {
      int n = getDim() - mNLagrangeMult;
      double lam = x[n];
      double v = 0.;
      mDNA->grad(x, g);
      for(int i = 0; i < n; i ++) {
        g[i] -= 2. * lam * x[i];
        v += x[i] * x[i];
      }
      g[n] = 1. - v;
      
    }
    
    double getFuncLipzConstant(double* A, double* B)
    {
      0.;    
    }
    
    double getGradLipzConstant(double* a, double* b)
    {
      double rv = 0., u = 0.;
      int n = getDim()- mNLagrangeMult;    
      for(int i = 0; i < n; i ++) {
        double v = getGIJ(i, i, a, b);
        if(i != 0) {
          v += getGIJ(i, i - 1, a, b);
        } else if(i != (n - 1)) {
          v += getGIJ(i, i + 1, a, b);
        }
        v += getGIJ(i, n, a, b);
        if(v > rv)
          rv = v;
        u += BNBMAX(BNBABS(a[i]), BNBABS(b[i]));
      }
      rv = BNBMAX(u, rv);
      return rv;      
    }
    
    double getGradCompLipzConstant(double* a, double* b, int i)
    {
      double rv, q = 0.;
      int n = getDim() - mNLagrangeMult;
      
      if(i < n) {
        q = BNBSQR(getGIJ(i, i, a, b));
        if(i != 0) {
          q += BNBSQR(getGIJ(i, i - 1, a, b));
        } else if(i != (n - 1)) {
          q += BNBSQR(getGIJ(i, i + 1, a, b));
        }
        q += BNBSQR(getGIJ(i, n, a, b));
      } else {
        for(int j = 0; j < n; j ++) {
          q += BNBMAX(BNBSQR(a[j]), BNBSQR(b[j]));
        }
        q *= 4.;        
      }
      rv = sqrt(q);
      return rv;
    }
    
  private:
    
    double getGIJ(int i, int j, double* a, double* b)
    {
      double rv, xa, xb, nu, kap;
      int n = getDim() - mNLagrangeMult;
      double maxlam = BNBMAX(BNBABS(a[n]), BNBABS(b[n]));
      if(j == i) {
        double A, B, C;
        xa = a[i];
        xb = b[i];
        nu = (*(mDNA->getDiag()))[i];
        kap = (*(mDNA->getKap()))[i];        
        A = BNBABS(nu  - 3 * kap * xa * xa);
        B = BNBABS(nu  - 3 * kap * xb * xb);
        C = BNBMAX(A, B);
        if((xa < 0) && (xb > 0)) 
          rv = BNBMAX(BNBABS(nu),C);
        else 
          rv = C;
        rv += maxlam;
      } else if(j == (i - 1)) {
        rv = BNBABS((*(mDNA->getSDiag()))[i - 1]);
      } else if(j == (i + 1)) {
        rv = BNBABS((*(mDNA->getSDiag()))[i]);
      } else if(i == n) {
        if(j == n)
          rv = 0.;
        else
          rv = BNBMAX(BNBABS(a[j]), BNBABS(b[j]));
      } else if(j == n) {
        if(i == n)
          rv = 0.;
        else
          rv = BNBMAX(BNBABS(a[i]), BNBABS(b[i]));
      }
      else {
        rv = 0.;
      }
      rv *= 2.; 
      return rv;
    }
        
    int mNLagrangeMult;
    DNA* mDNA;
};

#endif
