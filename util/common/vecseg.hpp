#ifndef __VECSEG_HPP__
#define __VECSEG_HPP__

#include <stdio.h>
#include "bnberrcheck.hpp"
#include "utilmacro.hpp"
#include "segment.hpp"
/**
  * Vector of non-overlapping open segments
  */

template <int MAX_VECSEG_SIZE, class FT> class VecSeg {
  public:

    VecSeg()
    {
      mSize = 0;
    }

    VecSeg(const VecSeg<MAX_VECSEG_SIZE, FT> &vs)
    {
      copy(vs);
    }


    VecSeg<MAX_VECSEG_SIZE, FT>& operator = (const VecSeg<MAX_VECSEG_SIZE, FT> &vs)
    {
      copy(vs);
      return *this;
    }

    /**
      * Push new segment
      *
      * @param s new segment
      */
    void push(Segment<FT>& s) 
    {
      Segment<FT> ns;
      FT a = s.mA;
      FT b = s.mB;
      ns.mA = a;
      ns.mB = b;
      BNB_ASSERT(mSize < MAX_VECSEG_SIZE);
      if(mSize == 0) {
	mArrSeg[0] = ns;
	mSize ++;
      } else {
	int l = -1, k = mSize;
	for(int i = 0; i < mSize; i ++) {
	  if(mArrSeg[i].mB <= a) 
	    l = i;
	  if(mArrSeg[i].mA >= b){
	    k = i;
	    break;
	  }
	}
	if((l + 1) <= (k - 1)) {
	  ns.mA = BNBMIN(ns.mA, mArrSeg[l+1].mA);
	  ns.mB = BNBMAX(ns.mB, mArrSeg[k-1].mB);
	  erase(l+1, k - 1);
	}
	insert(l, ns);
      }
    }
    /**
      * Prints vector of segments
      */
    void print() const
    {
      for(int i = 0; i < mSize; i++) {
	printf("(%lf, %lf) ", mArrSeg[i].mA, mArrSeg[i].mB);
      }
      printf("\n");
    }

    /**
      * Retrieves vector size
      * @return vectore size
      */
    int size() const
    {
      return mSize;
    }

    /**
      * Retrieve element
      * @param i elements index
      * @return segment 
      */
    Segment<FT> at(int i) const
    {
      return mArrSeg[i];
    }

  private:
    void erase(int b, int e)
    {
      int shft;
      shft = e - b + 1;
      BNB_ASSERT(shft > 0);
      for(int i = e + 1; i < mSize; i ++) {
	mArrSeg[i - shft] = mArrSeg[i];
      }
      mSize -= shft;
    }

    void insert(int aft, Segment<FT>& seg)
    {
      for(int i = mSize; i > (aft + 1); i --) {
	mArrSeg[i] = mArrSeg[i - 1];
      }
      mArrSeg[aft + 1] = seg;
      mSize ++;
    }

    void copy(const VecSeg<MAX_VECSEG_SIZE, FT>& vs) 
    {
      mSize = vs.mSize;
      for(int i = 0; i < mSize; i ++) {
	mArrSeg[i] = vs.mArrSeg[i];
      }
    }

    int mSize;
    Segment<FT> mArrSeg[MAX_VECSEG_SIZE];
};

#endif
