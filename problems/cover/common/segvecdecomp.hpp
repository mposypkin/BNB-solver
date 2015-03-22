#ifndef __SEGVECDECOMP_HPP__
#define __SEGVECDECOMP_HPP__

#include "go.hpp"
#include <util/common/gvector.hpp>

template <typename Factory> class SegVecDecomp : public Factory::Discarder 
{
  public:

    typedef typename Factory::ValueType FT;

    std::string getName() const 
    {
      return "SegVecDecomp";
    }

    void discard(typename Factory::Set& s, GVector<typename Factory::Set > &sv, GVector<typename Factory::Solution > &solv, FT record, BNBBranchInfo* info) {
      int n = s.mA.size();
      int I = s.mSplitI;
      int m = s.mSegments.size();

      if(m > 0) {
	for(int i = 0; i <= m; i ++) {
	  typename Factory::Set  sn(n);
	  bool put = false;
	  for(int j = 0; j < n; j ++) {
	    sn.mA[j] = s.mA[j];
	    sn.mB[j] = s.mB[j];
	  }
	  if(i == 0) {
	    Segment<FT> seg = s.mSegments.at(i);
	    if(s.mA[I] < seg.mA) {
	      sn.mB[I] = seg.mA;
	      put = true;
	    }
	  } else if(i == m) {
	    Segment<FT> pseg = s.mSegments.at(i-1);
	    if(s.mB[I] > pseg.mB) {
	      sn.mA[I] = pseg.mB;
	      put = true;
	    }
	  } else {
	    Segment<FT> pseg = s.mSegments.at(i-1);
	    Segment<FT> seg = s.mSegments.at(i);
	    sn.mA[I] = pseg.mB;
	    sn.mB[I] = seg.mA;
	    put = true;
	  }
	  if(put) {
	    sn.mSplitI = BoxUtils::getLongestEdge(sn);	
	    sv.push_back(sn);
	  }
	}
	if(sv.size() == 1) {
	  typename Factory::Set ss;
	  ss = sv[0];
	  sv.pop_back();
	  split(ss, sv);
	}
      } else {
	split(s, sv);
      }
      if(!sv.empty())
	info->mBranched ++;
    }
  private:

    void split(const typename Factory::Set &s, GVector<typename Factory::Set > &sv)
    {
      int n = s.mA.size();
      int I = s.mSplitI;
      FT mid = 0.5 * (s.mA[I] + s.mB[I]);
      typename Factory::Set  sl(n), sr(n);
      for(int i = 0; i < n; i ++) {
	sl.mA[i] = sr.mA[i] = s.mA[i];
	sl.mB[i] = sr.mB[i] = s.mB[i];
      }
      sl.mB[I] = mid;
      sr.mA[I] = mid;
      sl.mSplitI = BoxUtils::getLongestEdge(sl);
      sr.mSplitI = BoxUtils::getLongestEdge(sr);
      sv.push_back(sr);
      sv.push_back(sl);
    }
};
#endif
