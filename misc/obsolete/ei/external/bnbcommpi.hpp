/* 
 * File:   bnbcommpi.hpp
 * Author: posypkin
 *
 * Created on September 2, 2011, 10:41 AM
 */

#ifndef BNBCOMMPI_HPP
#define	BNBCOMMPI_HPP

#include <util/aclib/mpiaclib.hpp>

#include "bnbcommand.hpp"


template <> void AbstractCommunicator::pack <BNBCommand> (const BNBCommand * cmd)
{
  pack(&(cmd->mName));
  pack(&(cmd->mId));
  pack(&(cmd->mType));
  pack(&(cmd->mStatus));
  pack(&(cmd->mDsc));
  pack(&(cmd->mCode));
}

template <> void AbstractCommunicator::unpack <BNBCommand> (BNBCommand * cmd)
{
  unpack(&(cmd->mName));
  unpack(&(cmd->mId));
  unpack(&(cmd->mType));
  unpack(&(cmd->mStatus));
  unpack(&(cmd->mDsc));
  unpack(&(cmd->mCode));
}



#endif	/* BNBCOMMPI_HPP */

