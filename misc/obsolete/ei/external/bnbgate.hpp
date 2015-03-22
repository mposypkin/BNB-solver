#ifndef _BNBGATE_HPP_
#define _BNBGATE_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbgate.hpp
 *
 * A gate to the "outside" world.
 */

#include <stdlib.h>
#include <string>
#include "bnbcommand.hpp"
#include "bnbcmdtags.hpp"
#include "bnbcmdparser.hpp"


/**
 * Identifier of the gate in the global communicator
 */
#define BNB_GATE_ID_ATTR "gate"


class BNBGate 
{
public:


  /**
   * Open gate
   *
   */
  virtual void open() = 0;


/**
 * Checks whether data is ready to retrieve
 *
 * @return true if data is ready
 *
 */
  virtual bool probe() = 0;

/**
 * Retrieves next command from file or stream
 *
 * @param cmd command to fill
 *
 * @return false if no command retrieved, true otherwise
 *
 */
  virtual bool get(BNBCommand & cmd) = 0;

/**
 * Put command to the output file
 *
 * @param cmd
 *
 */
  virtual void put(BNBCommand & cmd) = 0;


  /**
   * Close gate
   *   
   */
  virtual void close() = 0;

};

#endif
