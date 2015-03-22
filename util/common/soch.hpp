#ifndef _SOCH_HPP_
#define _SOCH_HPP_
/**
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file soch.hpp
 */

/**
 * Generates all "sochetanija"
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */

#include <stdio.h>
#include <stdlib.h>

class Soch
{
public:

/**
 * The constructor
 *
 * @param n from 
 *
 * @param k how may to choose
 *
 */
  Soch(int n, int k)
  {
    mN = n;
    mK = k;
    mS = new int [k + 1];
    for(int i = 1; i <= k; i ++)
      mS[i] = i;
    mS[k] --;
  }


  int* next()
  {
    int i;
    for(i = mK; (i >= 1) && (mS[i] == mN - mK + i) ; i --);
    if(i == 0)
      return NULL;
    else {
      mS[i] ++;
      for(int j = i + 1; j <= mK; j ++)
	mS[j] = mS[i] + j - i;
      return mS + 1;
    }
  }

/**
 * Destructor
 *
 */
  ~Soch()
  {
    delete [] mS ;
  }
  
private:

/**
 * From
 */
  int mN;

/**
 * How much to choose
 */
  int mK;

/**
 * Where to store 
 */
  static int* mS;

};



#endif
