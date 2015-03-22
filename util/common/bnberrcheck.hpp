#ifndef _BNBERRCHECK_HPP_
#define _BNBERRCHECK_HPP_


/**
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnberrcheck.hpp
 *
 * The support for error checking at runtime.
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * Information message
 */
#define BNB_INFO_REPORT(str) {\
  fprintf(stderr, "INFO: %s at %s:%d\n", str, __FILE__, __LINE__);\
  fflush(stderr);\
} 

/**
 * Error message
 */
#define BNB_ERROR_REPORT(str) {\
  fprintf(stderr, "ERROR: %s at %s:%d\n", str, __FILE__, __LINE__);\
  fflush(stderr);\
  exit(-1);\
} 

/**
 * Assertion
 */
#define BNB_ASSERT(p){\
  if((p) == 0){\
    fprintf(stderr, "ASSERTION FALIED at %s:%d\n", __FILE__, __LINE__);\
    fflush(stderr);\
    exit(-1);\
  }\
}

#endif
