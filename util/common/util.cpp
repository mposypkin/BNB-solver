
#include <limits.h>
#include "util.hpp"


/**
 * Return maximum for the int
 */
template <> int getMaxValue <int> ()
{
  return INT_MAX;
}

/**
 * Return minimum for the int
 */
template <> int getMinValue <int> ()
{
  return -INT_MAX;
}


/**
 * Return maximum for the double
 */
template <> double getMaxValue <double> ()
{
  return DBL_MAX;
}

/**
 * Return minimum for the double
 */
template <> double getMinValue <double> ()
{
  return -DBL_MAX;
}

/**
 * Return maximum for the long double
 */
template <> long double getMaxValue <long double> ()
{
    return DBL_MAX;
}

/**
   * Return minimum for the long double
    */
template <> long double getMinValue <long double> ()
{
    return -DBL_MAX;
}


