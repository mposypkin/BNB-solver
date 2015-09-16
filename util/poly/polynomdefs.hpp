#ifndef __POLYNOMDEFS_HPP__
#define __POLYNOMDEFS_HPP__

/**
  * Basic polynom definitions
  */

// Maximal number of polynomial variables
#define MAX_POLYNOM_VARS 16

// Maximal degree of plynomial
#define MAX_POLYNOM_DEG 16

// Maximal number of monoms in a polynomial
#define MAX_MONOMS 4096

// Length of the cached degrees matrix row
#define DEG_MATRIX_ROW_LEN (MAX_POLYNOM_DEG  + 2)

// Length of the cached degrees intervals matrix row
#define DEG_INTERVAL_MATRIX_ROW_LEN ((MAX_POLYNOM_DEG + 1) * 4 + 1)
#endif
