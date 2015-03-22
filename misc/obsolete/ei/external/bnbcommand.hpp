#ifndef _BNBCOMMAND_HPP_
#define _BNBCOMMAND_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbcommand.hpp
 *
 * Commands for BNBSolver
 */
#include <stdio.h>
#include <string>
#include <util/aclib/aclib.hpp>

#include "bnbprotocol.hpp"


class BNBCommand {

  public:

    /**
     * The constructor 
     * 
     * @param name command name
     *
     * @param id command id
     *
     * @param  typ command type
     *
     * @param dsc command description
     *
     */
    BNBCommand(std::string name = BNB_HELLO, std::string id = "", std::string typ = BNB_ASYNC, std::string dsc = "", std::string code = "")
    {
      mName = name;    
      mId = id;
      mType = typ;
      mDsc = dsc;
      mCode = code;
    }

  /**
     * Command name
   */
    std::string mName;
  
  /**
     * Command identifier
   */
    std::string mId;

  /**
     * Command type
   */
    std::string mType;

	      
  /**
   * Command status
   */
    std::string mStatus;

  /**
   * Command descriptor
   */
    std::string mDsc;
    
  /**
   * Program code
   */
    std::string mCode;
};




#endif
