#ifndef __INFINI_PAR__
#define __INFINI_PAR__

#include "infini.hpp"
#include <util/aclib/aclib.hpp>

/**
 * Packs number to the send buffer
 *
 * @param id pointer to the initial data
 *
 */
template <> void AbstractCommunicator::pack <InfiniInt> (const InfiniInt * num)
{
  pack(&(num->mData));
}

/**
 * Unpacks initial data from the send buffer
 *
 * @param id pointer to the initial data
 *
 */
template <> void AbstractCommunicator::unpack <InfiniInt> (InfiniInt * num)
{
  unpack(&(num->mData));
}
#endif
