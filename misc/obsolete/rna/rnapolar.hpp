#ifndef __RNAPOLAR_HPP__
#define __RNAPOLAR_HPP__

#include <util/common/util.hpp>
#include <problems/go/lipzobjective.hpp>
#include "rnabase.hpp"
#include <util/common/polar.hpp>

class RNAPolar : public LipzObjective<double>  {
  public:
    
    RNAPolar() : LipzObjective<double>()
    {
      mRNABase = NULL;
    }
    
    void setRNABase(RNABase<double>* base)
    {
      mRNABase = base;
      setDim(mRNABase->getDim() - 1);
    }
    
    double func(const double* phi)
    {
      int n;
      double v;
      BNB_ASSERT(mRNABase);
      n = mRNABase->getDim() - 1;
      mX.lazyAlloc(n + 1);
      Polar::polar2cart(n + 1, 1., phi, (double*)mX);
      v = mRNABase->func(mX);
      return v;
    }
   
   void grad(const double* phi, double* g)
   {
     int n;
     BNB_ASSERT(mRNABase);
     n = mRNABase->getDim() - 1;
     mX.lazyAlloc(n + 1);
     mG.lazyAlloc(n + 1);
     Polar::polar2cart(n + 1, 1., phi, (double*)mX);
     mRNABase->grad(mX, mG);
     Polar::polarAngleGrad(n + 1, 1., phi, (double*)mG, g);
   }
   
   
   double getFuncLipzConstant(double* a, double* b)
   {
     return .45;
   }
   
   double getGradLipzConstant(double* a, double* b)
   {
     return .75;
   }
   
   double getGradCompLipzConstant(double* a, double* b, int i)
   {
     return .75;
   }
  private:
    SmartArrayPtr<double> mX;
    SmartArrayPtr<double> mG;
    RNABase<double> *mRNABase;
};

#endif
