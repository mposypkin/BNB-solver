#ifndef __DECOMPOSER_HPP__
#define __DECOMPOSER_HPP__

#include "go.hpp"
#include <util/common/gvector.hpp>

/**
  * Maximal tree layer for decomposition
  */
#define MAX_DECOMP_LEVEL 4

template <typename Factory> class Decomposer : public Factory::Discarder 
{
  public:

    Decomposer()
    {
      mBallLevel = 0;
    }

    std::string getName() const
    {
      return "Standard Decomposer";
    }


    /**
      * Set the max level of decomposition for ball extraction
      * @param level the desired level
      */
    void setBallExcludeLevel(int level) 
    {
      if(level > MAX_DECOMP_LEVEL) {
	BNB_ERROR_REPORT("Ball exclude level exceeds maximal decomposition level\n");
      }
      mBallLevel = level;
    }

    void discard(typename Factory::Set& s, GVector<typename Factory::Set > &sv, GVector<typename Factory::Solution > &solv, typename Factory::Solution &sol, BNBBranchInfo* info)
    {
//      discard_new(s, sv, solv, 0, info);
//      discard_old(s, sv, solv, 0, info);
    }

    void discard(typename Factory::Set& s, GVector<typename Factory::Set > &sv, GVector<typename Factory::Solution > &solv, double record, BNBBranchInfo* info) {
#if 1
      discard_new(s, sv, solv, record, info);
#else
      discard_old(s, sv, solv, record, info);
#endif
    }

#if 0
    void discard_old(typename Factory::Set& s, GVector<typename Factory::Set > &sv, GVector<typename Factory::Solution > &solv, double record, BNBBranchInfo* info)
    {
      int n = s.mA.size();
      typename Factory::Set  sl(n), sr(n);
      int I = 0;
      double dist, mid, q = 0.;      
      dist = getMinValue<double>();

      for(int i = 0; i < n; i ++) {
	double d;

	d = s.mB[i] - s.mA[i];
	if(d > dist) {
	  dist = d;
	  I = i;
	} 
	q += d * d;
      }
      q -= dist * dist;
      q *= 0.25;
      for(int i = 0; i < n; i ++) {
          sl.mA[i] = sr.mA[i] = s.mA[i];
          sl.mB[i] = sr.mB[i] = s.mB[i];
      }
      mid = 0.5 * (s.mA[I] + s.mB[I]);
      double delt = 0.;
      if(mOptions & Options::EXCLUDE_BALL) {
	delt = sqrt(BNBMAX(0., s.mDiscardRho * s.mDiscardRho - q));
	printf("excluding %lf, rho = %lf\n", delt, s.mDiscardRho);
      }
      sl.mB[I] = mid - delt;
      sr.mA[I] = mid + delt;    
      sv.push_back(sr);
      sv.push_back(sl);
      info->mBranched ++;
    }
#endif

    void discard_new(typename Factory::Set& s, GVector<typename Factory::Set > &sv, GVector<typename Factory::Solution > &solv, double record, BNBBranchInfo* info)
    {
      int n = s.mA.size();
      int I;
      double e, emax, dist, mid, q = 0., dq, r;      
      struct R { int dir; double delt;};
      R dirs[MAX_DECOMP_LEVEL + 1];
      int dlevels = 0;
      for(int i = 0; i < MAX_DECOMP_LEVEL; i ++) {
	dirs[i].dir = -1;
	dirs[i].delt = 0;
      }

      r = s.mDiscardRho;
      I = 0;
      emax = 0.5 * (s.mB[0] - s.mA[0]);
      for(int i = 0; i < n; i ++) {
	e = 0.5 * (s.mB[i] - s.mA[i]);
	q += e * e;
	if(e > emax) {
	  emax = e;
	  I = i;
	}
      }

      for(int i = 0; i < n; i ++) {
	e = 0.5 * (s.mB[i] - s.mA[i]);
	dq = q - e * e;
	double rq = r * r;
	if(dq < rq){
	  double delt = sqrt(rq - dq);
	  int j = 0;
	  while(1) {
	    if(j >= mBallLevel)
	      break;
	    else if(j == dlevels) {
	      dirs[j].dir = i;
	      dirs[j].delt = delt;
	      dlevels ++;
	      break;
	    } else if(dirs[j].delt < delt){
	      for(int k = dlevels; k > j; k --) {
		dirs[k] = dirs[k - 1];
	      }
	      dirs[j].dir = i;
	      dirs[j].delt = delt;
	      dlevels ++;
	      dlevels = BNBMIN(dlevels, mBallLevel);
	      break;
	    } else 
	      j ++;
	  }
	}
      }
      for(int i = 0; i < dlevels; i ++) {
	printf("%d -> %lf %c", dirs[i].dir, dirs[i].delt, (i == dlevels - 1) ? '\n' : ',');
      }
      if(dlevels == 0) {
	dlevels = 1;
	dirs[0].dir = I;
      }
      for(int i = 0; i < (1 << dlevels); i ++) {
	typename Factory::Set sn(n);
	for(int j = 0; j < n; j ++) {
          sn.mA[j] = s.mA[j];
          sn.mB[j] = s.mB[j];
	}
	for(int j = 0; j < dlevels; j ++) {
	  double delt = dirs[j].delt;
	  int J = dirs[j].dir; 
	  double mid = 0.5 * (s.mA[J] + s.mB[J]);
	  if((1 << j) & i) {
	    sn.mA[J] = mid + delt;
	    sn.mB[J] = s.mB[J];
	  } else {
	    sn.mA[J] = s.mA[J];
	    sn.mB[J] = mid - delt;
	  }
	  /*
	  printf("delt = %lf, mid = %lf, J = %d\n", delt, mid, J);
	  printf("(%lf, %lf, %lf, %lf)\n", sn.mA[J], sn.mB[J], s.mA[J], s.mB[J]);
	  */
	}
	/*
	printf("Yum!" );
	for(int k = 0; k < n; k ++) {
	  printf("%d: [%lf, %lf], [%lf, %lf]\n", k, sn.mA[k], sn.mB[k], s.mA[k], s.mB[k]);
	  if(sn.mA[k] > sn.mB[k])
	    BNB_ERROR_REPORT("impossible!\n");
	}
	printf("Yam!\n");
	*/
	sv.push_back(sn);
      }
      info->mBranched ++;
    }

    unsigned int& getOptions()
    {
      return mOptions;
    }

  private:

    unsigned int mOptions;
    int mBallLevel;
};
#endif
