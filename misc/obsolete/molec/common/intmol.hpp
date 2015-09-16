#ifndef __INTMOL_HPP_
#define __INTMOL_HPP_

#include <problems/cover/common/intobj.hpp>
#include <util/common/vec.hpp>
#include <util/common/interval.hpp>
#include <problems/cover/common/lipzobjective.hpp>

#include "potent.hpp"
#include "molecutils.hpp"

template <typename FT> class IntervalMolec : public IntervalObjective <FT>,
 public LipzObjective <FT> {
  public:
    
    /**
     * Constructor
     * @param n number of atoms
     * @param d number of space dimensions
     * @param pot pointer to the potential
     */
    IntervalMolec(int n, int d, Potential<FT> *pot)
    {
      mN = n;
      mD = d;
      Objective<FT>::setDim(n * d);
      mPot = pot;
      mHessRowLB = (FT*)malloc(n * d * sizeof(FT));
      mHessRowUB = (FT*)malloc(n * d * sizeof(FT));
    }
    
    ~IntervalMolec()
    {
      free(mHessRowLB);
      free(mHessRowUB);
    }
    
    FT func(const FT* x)
    {
      return mPot->totalPotential(mN, mD, x);
    }
    
    void funcMin(FT* avec, FT* bvec, FT* minv)
    {
      FT r= 0.;
      for(int i = 0; i < mN; i ++){
        for(int j = i + 1; j < mN ; j ++) {
          FT dmin, dmax, e;
          dmin = evalSqrMinDistance(mD, avec + i * mD, bvec + i * mD, avec + j * mD, bvec + j * mD);
          dmax = evalSqrMaxDistance(mD, avec + i * mD, bvec + i * mD, avec + j * mD, bvec + j * mD);
          if(dmin > 1.0)
            e = mPot->getPotent(dmin);
          else if(dmax < 1.0)
            e = mPot->getPotent(dmax);
          else 
            e = mPot->getPotent(1.0);
          r += e;
        }
      }
      *minv = r;
    }
    
    void grad(const FT* x, FT* g)
    {
      mPot->totalGrad(mN, mD, x, g);
    }

    FT gradAndFunc(const FT* x, FT* g)
    {
      return mPot->totalGradAndPot(mN, mD, x, g);
    }

    virtual void gradI(FT* a, FT* b, FT* gmin, FT* gmax)
    {
      int n;
      n = mD * mN;
      for(int i = 0; i < n; i ++) {
        gmin[i] = 0;
        gmax[i] = 0;
      }
      for(int I = 0; I < mN; I ++) {
        int i;
        i = I * mD;
        for(int J = I + 1; J < mN; J ++) {
          int j;
          FT qmin, qmax, x, y; 
          j = J * mD;          
          qmin = evalSqrMinDistance(mD, a + i, b + i, a + j, b + j);
          qmax = evalSqrMaxDistance(mD, a + i, b + i, a + j, b + j);
          boundDf(qmin, qmax, &x, &y);

          for(int k = 0; k < mD; k ++) {
            int ik, jk;
            FT xikl, xiku, xjkl, xjku, c, d;

            
            ik = I * mD + k;
            jk = J * mD + k;
            xikl = a[ik];
            xiku = b[ik];
            xjkl = a[jk];
            xjku = b[jk];
            BNBInterval<FT>::diff(xikl, xiku, xjkl, xjku, &c, &d);
            BNBInterval<FT>::mult(c, d, x, y, &c, &d);
            BNBInterval<FT>::sum(gmin[ik], gmax[ik], c, d, gmin + ik, gmax + ik); 
            BNBInterval<FT>::diff(xjkl, xjku, xikl, xiku, &c, &d);
            BNBInterval<FT>::mult(c, d, x, y, &c, &d);
            BNBInterval<FT>::sum(gmin[jk], gmax[jk], c, d, gmin + jk, gmax + jk); 
          }
        }
      }
      FT alpha = 1.;
      VecUtils::vecMult(n, gmin, alpha * 2., gmin);
      VecUtils::vecMult(n, gmax, alpha * 2., gmax);

      //mPot->boundGrad(mN, mD, avec, bvec, lbound, ubound);
    }

    virtual FT getGradLipzConstant(FT* a, FT* b)
    {
      FT v;
      //v = honestGradLipzicConstant(a, b);
      //printf("hglc = %lf\n", v);      
      //v *= 0.1;
      v = 200000;
      return v;
    }
  
    virtual FT getFuncLipzConstant(FT* a, FT* b) {
      return 10.;
    }
    
    virtual FT getGradCompLipzConstant(FT* a, FT* b, int i)
    {
      FT v;
      v = honestGradComponentConstant(a, b, i);
      //printf("hgcc = %lf\n", v);
      //v *= 0.01;
      //v = 2.189;
      return v;
    }
    
   private:
     
     FT boundDf(FT a, FT b, FT* x, FT* y)
     {
       double q0;
       q0 = mPot->dfmax();
       if(a >= q0){
         *y = mPot->getDf(a);
         *x = mPot->getDf(b);
       } else if(b <= q0) {
         *x = mPot->getDf(a);
         *y = mPot->getDf(b);
       } else {
         *x = BNBMIN(mPot->getDf(a), mPot->getDf(b));
         *y = mPot->getDf(q0);
       }
     }
     
     FT boundDDf(FT a, FT b, FT* x, FT* y)
     {
       double q0;
       q0 = mPot->ddfmax();
       if(a >= q0){
         *x = mPot->getDDf(a);
         *y = mPot->getDDf(b);
       } else if(b <= q0) {
         *y = mPot->getDDf(a);
         *x = mPot->getDDf(b);
       } else {
         *y = BNBMAX(mPot->getDDf(a), mPot->getDDf(b));
         *x = mPot->getDDf(q0);
       }
     }
     
     FT honestGradLipzicConstant(FT* a, FT* b) 
     {
       FT nor;
       FT lnor;
       FT unor;
       int n;
       n = mN * mD;
       nor = 0.;
       lnor = 0.;
       unor = 0.;
       for(int i = 0; i < n; i ++) {
         fillHessRowBounds(a, b, i);
         FT nori = 0.;
	 FT lnori = mHessRowLB[i];
	 FT unori = mHessRowUB[i];
         for(int j = 0; j < n; j ++)  {
           nori += BNBMAX(BNBABS(mHessRowLB[j]), BNBABS(mHessRowUB[j]));
	   if(i != j) {
	     lnori -= BNBMAX(BNBABS(mHessRowLB[j]), BNBABS(mHessRowUB[j]));
	     unori += BNBMAX(BNBABS(mHessRowLB[j]), BNBABS(mHessRowUB[j]));
	   }
         }
         if(nori > nor) 
           nor = nori;
	 if(lnori < lnor)
	   lnor = lnori;
	 if(unori > unor)
	   unor = unori;
       }
       //printf("nor = %lf, lnor = %lf, unor = %lf\n", nor, lnor, unor);
       //return nor;
       return BNBABS(lnor);
     }
     
     FT honestGradComponentConstant(FT* a, FT* b, int comp) 
     {
       fillHessRowBounds(a, b, comp);
       int n;
       FT nor;
       n = mN * mD;
       nor = 0.;
       for(int i = 0; i < n; i ++){
         nor += BNBMAX(BNBSQR(mHessRowLB[i]), BNBSQR(mHessRowUB[i]));
       }
       return sqrt(nor);
     }
     
     void fillHessRowBounds(FT* a, FT* b, int row)
     {
       int I, J, i, k, n;
       I = row / mD;
       i = mD * I;
       k = row % mD;
       n = mN * mD;
       for(int t = 0; t < n; t ++) {
         mHessRowLB[t] = 0.;
         mHessRowUB[t] = 0.;
       }
       for(int J = 0; J < mN; J ++) {
         int j;
         j = mD * J;
         if(J != I) {
           int ik, jk;
           FT x, y, xikl, xiku, xjkl, xjku;
           FT qmin, qmax, dmin, dmax, ddmin, ddmax;
           qmin = evalSqrMinDistance(mD, a + i, b + i, a + j, b + j);
           qmax = evalSqrMaxDistance(mD, a + i, b + i, a + j, b + j);
           boundDf(qmin, qmax, &dmin, &dmax);
           boundDDf(qmin, qmax, &ddmin, &ddmax);
           ik = I * mD + k;
           jk = J * mD + k;
           xikl = a[ik];
           xiku = b[ik];
           xjkl = a[jk];
           xjku = b[jk];
           for(int l = 0; l < mD; l ++) {
             int il, jl;
             FT xill, xilu, xjll, xjlu;
             FT ck, dk, cl, dl, c, d;
             il = I * mD + l;
             jl = J * mD + l;
             xill = a[il];
             xilu = b[il];             
             xjll = a[jl];
             xjlu = b[jl];
             BNBInterval<FT>::diff(xikl, xiku, xjkl, xjku, &ck, &dk);
//             printf("%d, %d, %d\n", I, mD, l);
//              printf("%d: %lf, %lf, %lf, %lf\n", il, xill, xilu, xjll, xjlu);
             BNBInterval<FT>::diff(xill, xilu, xjll, xjlu, &cl, &dl);
//              printf("ck = %lf, dk = %lf, cl = %lf, dl = %lf\n", ck, dk, cl, dl);
             BNBInterval<FT>::mult(ck, dk, cl, dl, &c, &d);
             BNBInterval<FT>::mult(c, d, ddmin, ddmax, &c, &d);
             c *= 4.;
             d *= 4.;
             mHessRowLB[jl] = c;
             mHessRowUB[jl] = d;
             mHessRowLB[il] += c;
             mHessRowUB[il] += d;             
             if(l == k) {
               mHessRowLB[jk] = 2. * dmin;
               mHessRowUB[jk] = 2. * dmax;
               mHessRowLB[ik] += 2. * dmin;
               mHessRowUB[ik] += 2. * dmax;               
             }
           }           
         }
       }
     }
     
    Potential<FT>* mPot;    
    int mN;    
    int mD;
    FT* mHessRowLB;
    FT* mHessRowUB;
};
#endif
