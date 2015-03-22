#ifndef _BNBCHARGATE_HPP_
#define _BNBCHARGATE_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbgate.hpp
 *
 * A gate to the "outside" world for character devices.
 */

#include <stdlib.h>
#include <string>
#include "bnbcommand.hpp"
#include "bnbcmdtags.hpp"
#include "bnbcmdparser.hpp"
#include "bnbgate.hpp"



class BNBCharGate : public BNBGate
{
public:

/**
 * Retrieves next command from file
 *
 * @param cmd command to fill
 *
 * @return false if no command retrieved, true otherwise
 *
 */
  bool get(BNBCommand & cmd)
  {
    int c;
    std::string s;

    while(1) {
      c = readc();
      if(c == EOF)
	return false;
      else if(c == '\r')
        return false;
      else if(c == '\n')
        return false;        
      else if(c == BNB_COMMAND_START){
	s += c;
	break;
      }
    }

    while(1) {
      c = readc();
      if(c == EOF)
	return false;
      else if(c == BNB_COMMAND_END) {
	s += c;
	break;
      } else 
	s += c;
    }
    BNBCommandParser::fromString(s, cmd);
    return true;
  }

/**
 * Put command to the output file
 *
 * @param cmd
 *
 */
  void put(BNBCommand & cmd)
  {
    std::string s;
    
    BNBCommandParser::toString(cmd, s);
    writes(s.c_str(), s.size());
  }


private:
/**
 * Reads the input symbol
 *
 * @return read symbol
 *
 */
  virtual int readc() = 0;

/**
 * Writes byte seq the output buffer
 *
 * @param str byte seq
 *
 * @param sz number of bytes
 *
 */
  virtual void writes(const char* str, int sz) = 0;


};

#endif
