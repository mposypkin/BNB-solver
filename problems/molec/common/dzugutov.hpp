#ifndef __DZUGUTOV_HPP__
#define __DZUGUTOV_HPP__

/**
 * Dzugutov potential
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file dzugutov.hpp
 */

#include <math.h>
#include <util/common/interval.hpp>
#include <util/common/vec.hpp>
#include "molecutils.hpp"
#include "potent.hpp"

#define DZ_A 5.82
#define DZ_B 1.28
#define DZ_a 1.87
#define DZ_b 1.94
#define DZ_c 1.10
#define DZ_d 0.27
#define DZ_m 16.
 
template <class FloatType> class Dzugutov : public Potential <FloatType>
{
  public:
  /**
   * The constructor
   *
   * @param rho the rho parameter of the Morse potential
   */
    Dzugutov()
    {
    }

    FloatType potent(FloatType dsq)
    {
      FloatType r, e, v;
      r = sqrt(dsq);
      if(r > DZ_b) 
        v = 0;
      else {
        v = DZ_B * exp(DZ_d / (r - DZ_b));
        if(r <= DZ_a) {
          e = dsq * dsq;
          e *= e;
          e *= e;
          v += DZ_A * (1./e - DZ_B) * exp(DZ_c /(r - DZ_a));
        }
      } 
      return v;
    }

    FloatType df(FloatType dsq)
    {
      FloatType r, e, g, q, u, E;
      
      r = sqrt(dsq);      
      if(r >= DZ_b) 
        u = 0.;
      else {        
        q = r - DZ_b;
        E = exp(DZ_d / q);
        u = - DZ_B * DZ_d * E  / (q * q);
        if(r <= DZ_a) {
          e = dsq * dsq;
          e *= e;
          e *= e;
          g = e * r;
          q = r - DZ_a;
          E = exp(DZ_c /q);
          u += - DZ_A * E * (DZ_m / g + (1./e - DZ_B) * DZ_c/(q*q));
        }
      } 
      return u/r;
    }
  
    void fandf(FloatType dsq, FloatType* fval, FloatType* der)
    {
      FloatType r, e, g, q, u, v, E;
      r = sqrt(dsq);
      if(r > DZ_b) {
        u = 0.;
        v = 0.;
      }
      else {
        q = r - DZ_b;
        E = exp(DZ_d / q);
        v = DZ_B * E;
        u = - DZ_B * DZ_d * E  / (q * q);
        if(r <= DZ_a) {
          e = dsq * dsq;
          e *= e;
          e *= e;
          g = e * r;
          q = r - DZ_a;
          E = exp(DZ_c /q);
          u += - DZ_A * E * (DZ_m / g + (1./e - DZ_B) * DZ_c/(q*q));
          v += DZ_A * (1./e - DZ_B) * E;
        }
      }
      *fval = v;
      *der = u/r;       
    }

    FloatType ddf(FloatType q) 
    {
     }
  
    FloatType dfmax()
    {
    }
  
    FloatType ddfmax()
    {
      BNB_ERROR_REPORT("argmax for 2nd derivative is not yet implemented");
    }
  

    FloatType leftbound()
    {
      return 0.0;
    }
  
  
    void boundGrad(int N, int D, FloatType* a, FloatType* b, FloatType* gmin, FloatType* gmax)
    {
    }

  
   private:
  
};
#endif

