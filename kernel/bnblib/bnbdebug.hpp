#ifndef _BNBDEBUG_HPP_
#define _BNBDEBUG_HPP_
/**
 * Useful class for degbugging parallel programs
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbdebug.hpp
 */

#include <stdio.h>
#include <stdlib.h>
#include <util/common/bnblog.hpp>

class BNBDebug
{
public:
/**
 * Define a point to wait a given bumber of seconds
 *
 * @param seconds number of seconds
 * @param label the string label
 */
  static void waitPoint(const int seconds, const char* p = "")
  {
    printf("WP %s: ", p);
    for(int g = 0; g < seconds; g ++) {
      sleep(1);
      printf(".");
      fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
  }

};

#endif
