#ifndef __multicovGDISC_HPP__
#define __multicovGDISC_HPP__

#include <math.h>
#include "multigo.hpp"
#include "multicovobj.hpp"
#include <util/common/gvector.hpp>
/**
 * A discarder based on a Lipzic formula for a function
 */
class multicovGDiscarder : public MGOFactory::Discarder 
{
  
  public:
    
    multicovGDiscarder()
    {
      mObj = NULL;
    }
    
    /**
     * Setup objective pointer
     * @param obj objective
     */
    void setObjective(multicovObjective* obj)
    {
      mObj = obj;
    }
    
    /**
     * Setup precision
     * @param eps epsilon vector
     */
    void setEps(double *eps)
    {
      mEps = eps;
    }
    
    void discard( MGOFactory::Set& s, GVector< MGOFactory::Set >& sv, GVector< MGOFactory::Solution>& solv, MGOFactory::Solution &incumbent, BNBBranchInfo* binfo)
    {
      double l, v, r;
      double C[MAX_NUMBER_OF_PARAMS];
      double G[MAX_NUMBER_OF_CRITERIA * MAX_NUMBER_OF_PARAMS];
      double F[MAX_NUMBER_OF_CRITERIA];
      double L[MAX_NUMBER_OF_CRITERIA];
      int n = mObj->getNumOfParams();
      int m = mObj->getNumOfCriteria();
      MGOFactory::Point bound(n, m);
      int imax = 0;
      double rimax = 0;
      BNB_ASSERT(mObj);
      r = 0;
      for(int i = 0; i < n; i ++) {
	double ri;
        C[i] = 0.5 * (s.mA[i] + s.mB[i]);        
	ri = s.mB[i] - C[i];
        r += BNBSQR(ri);
	if(ri > rimax) {
	  rimax = ri;
	  imax = i;
	} 

	//printf("%d: (%lf, %lf) ri = %lf, rimax = %lf\n", i, s.mA[i], s.mB[i],ri, rimax);
      }
      r = sqrt(r);
      mObj->func(C, F);
      mObj->grad(C, G);
      mObj->getGradLipConstants(C, L);
      for(int i = 0; i < m; i ++) {
	double gs = 0.;
	for(int j = 0; j < n; j ++) {
	  gs += BNBABS(G[i * m + j] * (s.mB[j] - C[j]));
	}
	bound.mCriteria[i] = F[i] - gs - 0.5 * rimax * rimax * L[i] +  mEps[i];
      }
      bool rv = false;
      for(std::vector<MGOFactory::Point>::const_iterator i = incumbent.mPoints.begin(); i != incumbent.mPoints.end(); i ++) {
	if((*i) <= bound) {
	  rv = true;
	  break;
	}
      }

      if(rv) {
	binfo->mDiscardedByRecord ++;
      } else {
	sv.push_back(s);
      }


	/*
	printf("estim = %lf, record = %lf, fv = %lf, eps = %lf\n", estim, record, v, mEps);
	printf("rimax = %lf, rbox = %lf, rboxi = %lf, re = %lf \n", rimax, r, sqrt(r * r - rimax * rimax), (v - record + mEps) / l);
	printf("rv = %s\n", rv ? "true" : "false");
	*/
	
    }
    
  private:
    
    double* mEps;
    multicovObjective* mObj;
};
#endif
