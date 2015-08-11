#ifndef __RANDOM_HPP__
#define __RANDOM_HPP__

#include <stdlib.h>
#include <math.h>
#include <iostream>

/**
 * Utilities for pseudo-random numbers and sequences generation
 */

/**
 * A wrapper class over simple number generator
 */
class BnbRandom {
public:
    
    /**
     * Retrieve random number in [a, b] segment
     * @return 
     */
    template <class FT> static FT get(FT a, FT b) {
        FT r = (FT) rand() / (FT) RAND_MAX;
        FT q = a + (b - a) *r;
        return q;
    }
    
};

/**
 * Implements random tries with given probability
 */
class Coin {
  public:
  /**
   * Throws coin
   * @param prob the probability of one side (from 0 to 1 )
   * @return true if value was obtained
   */
    static bool throwCoin(double prob = 0.5) {
      int r;
      r = rand();
      if((((double) r) / (double)(RAND_MAX)) < prob)
        return true;
      else 
        return false;
    };
};

/**
 * Gaussian (normal) mean value 0, deviation 1
 */
class Gaussian {
  public:
    Gaussian() {
      mSeqLen = 12;
    }
    
    /**
     * Set the lengths of uniform numbers sequence
     * @param len length
     */
    void setSeqLen(int len) {
      mSeqLen = len;
    }
    
    /**
     * Get next random number in sequence
     * @return random number
     */
    double getRand() {
      double v;
      v = 0.;
      for(int i = 0; i < mSeqLen; i ++) {
        double u;
        u = ((double)rand())/ ((double)RAND_MAX);
        v += u;
      }
      return v - (0.5 * ((double)mSeqLen));
    }
    
  private:
    
    int mSeqLen;
};

/**
 * Generates uniform sphere distribution
 */
class UniformSphereDistribution {
  public:
    /**
     * Generate next point
     * @param n space dimensionality
     * @param radius sphere radius
     * @param center sphere center (null for 0)
     * @param x resulting vector point
     */
    static void nextPoint(int n, double radius, double * center, double* x) {
      double norm;
      Gaussian gauss;
      gauss.setSeqLen(n);
      norm = 0.;
      for(int j = 0; j < n; j ++) {
        x[j] = gauss.getRand();
        norm += x[j] * x[j];;
      }
      double a;
      a = radius / sqrt(norm);
      for(int j = 0; j < n; j ++) {
        x[j] *= a;
      }
      if(center) {
        for(int j = 0; j < n; j ++) {
          x[j] += center[j];
        }
      }
    }
};
#endif
