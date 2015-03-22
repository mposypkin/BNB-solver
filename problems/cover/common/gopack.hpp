#ifndef _GOPACK_HPP_
#define _GOPACK_HPP_
/**
 * Packing utils for global optimization.
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file gopack.hpp
 */


#include <util/aclib/aclib.hpp>
#include "go.hpp"

/**
 * Packs set data into the send buffer
 *
 * @param s set data
 *
 */
template <> void  AbstractCommunicator::pack <GOFactory<double>::Set> (const GOFactory<double>::Set * s)
{
  int n = s->mDim;
  pack (&n);
//  pack (&(s->mFval));
  pack (&(s->mEstim));
  pack (&(s->mSplitI));
  packArray((double*)s->mA, s->mDim);
  packArray((double*)s->mB, s->mDim);
  /*
  for(int i = 0; i < n; i ++) {
    pack(&(s.mSegments[i].mA));
    pack(&(s.mSegments[i].mB));
  }
  */
}

/**
 * Unpacks set data into the send buffer
 *
 * @param s set data
 *
 */
template <> void   AbstractCommunicator::unpack <GOFactory<double>::Set> (GOFactory<double>::Set * s)
{

  int dim;
  unpack(&dim);
  GOFactory<double>::Set s1(dim);  

  //unpack (&(s1.mFval));
  unpack (&(s1.mEstim));
  unpack (&(s1.mSplitI));
  unpackArray((double*)s1.mA, s1.mDim);
  unpackArray((double*)s1.mB, s1.mDim);
  *s = s1;
}

/**
 * Packs solution data into the send buffer
 *
 * @param s set data
 *
 */
template <> void  AbstractCommunicator::pack <GOFactory<double>::Solution> (const GOFactory<double>::Solution * s)
{
  pack (&(s->mDim));
  pack (&(s->mFval));
  packArray((double*)s->mX, s->mDim);
}

/**
 * Unpack solution data into the send buffer
 *
 * @param s set data
 *
 */
template <> void   AbstractCommunicator::unpack <GOFactory<double>::Solution> (GOFactory<double>::Solution * s)
{

  int dim;
  unpack(&dim);
  GOFactory<double>::Solution s1(dim);  

  unpack (&(s1.mFval));
  unpackArray((double*)s1.mX, s1.mDim);
  *s = s1;
}

#endif
