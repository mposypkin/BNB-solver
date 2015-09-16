#ifndef _POTENT_HPP_
#define _POTENT_HPP_

#include <util/common/util.hpp>
#include <util/common/bnberrcheck.hpp>

#define MAXD 3

/**
 *
 * Base class for molecular potential
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file potent.hpp
 */

template <class FT> class Potential {

public:

  /**
   * The constructor
   */
  Potential()
  {
    mScale = 1.;
  }
  
  /**
   * Set up scaling factor
   * @param scale scaling factor
   */
  void setScale(FT scale)
  {
    mScale = scale;
  }
  
  /**
   * Calculates potential for a given sqare distance
   * @param dsq square distance
   */
  FT getPotent(FT dsq)
  {
    FT l, v;
    l = leftbound();
    if(dsq >= l) {
       v = potent(dsq); 
    } else {       
       v = potent(l) + (dsq - l) * df(l);
    }
    return v;
  }
  
  /**
   * Calculate first derivative at a given point
   * @param dsq square distance
   */
  FT getDf(FT dsq)
  {
    FT l;
    l = leftbound();
    if(dsq >= l)
      return df(dsq);
    else
      return df(l);
  }
  
  /**
   * Calculate second derivative
   * @param dsq square distance
   * @return dsq square distance
   */
  FT getDDf(FT dsq)
  {
    FT l;
    l = leftbound();
    if(dsq >= l)
      return ddf(dsq);
    else
      return 0.;    
  }
  
  /**
   * Total potential value
   *
   * @param N number of atoms 
   *
   * @param d space dimension
   *
   * @param x cartesian coorsinates
   *
   * @return total potential
   *
   */
  FT totalPotential(int N, int d, const FT* x)
  {
    FT r = 0.;
    int n;

    n = d * N;
    for(int i = 0; i < n; i += d) {
      for(int j = i + d; j < n; j += d) {
        FT dist = 0.;
	
        dist = 0.;
        for(int k = 0; k < d; k ++) 
          dist += BNBSQR(x[i + k] - x[j + k]); 
        r += getPotent(dist);      
      }
    }
    return r;
  }
  
  
  
/**
   * Evaluate potential first derivative (gradient)
   *
   * @param N number of atoms
   *
   * @param d space dimension
   *
   * @param x argument
   *
   * @param grad result
 */
  void totalGrad(int N, int d, const FT* x, FT* grad)
  {    
    int n;
    n = N * d;
    for(int i = 0; i < n; i ++)
      grad[i] = 0.;
    for(int I = 0; I < N - 1; I ++) {
      int i;
      i = I * d;
      for(int J = I + 1; J < N; J ++) {
        FT q, der;
        int j;
        j = J * d;
        q = 0.;
        for(int k = 0; k < d; k ++) 
          q += BNBSQR( x[i + k] - x[j + k]);
        der = getDf(q);
        for(int k = 0; k < d; k ++) {
          grad[i + k] += der * (x[i + k] - x[j + k]);
          grad[j + k] += der * (x[j + k] - x[i + k]);          
        }
      }
    }
    for(int i = 0; i < n; i ++)
      grad[i] *= 2.;    
  }
  
/**
   * Evaluate potential and its first derivative (gradient)
   *
   * @param N number of atoms
   *
   * @param d space dimension
   *
   * @param x argument
   *
   * @param grad result
   * @return potential value 
 */
  FT totalGradAndPot(int N, int d, const FT* x, FT* grad)
  {    
    int n;
    double r = 0.;
    n = N * d;
    for(int i = 0; i < n; i ++)
      grad[i] = 0.;
    for(int I = 0; I < N - 1; I ++) {
      int i;
      double X[MAXD];
      
      i = I * d;
      for(int k = 0; k < d; k ++) 
        X[k] = x[i + k];
      
      for(int J = I + 1; J < N; J ++) {
        FT q, der, pot;
        FT Y[MAXD];
        int j;
        j = J * d;
        q = 0.;
        for(int k = 0; k < d; k ++) {
          double t;
          t = X[k] - x[j + k];
          q += t * t;
          Y[k] = t;
        }
        /*
        for(int k = 0; k < d; k ++) 
          q += BNBSQR((x[i + k] - x[j + k]));
        */
        fandf(q, &pot, &der);        
        r += pot;      
/*        for(int k = 0; k < d; k ++) {
          grad[i + k] += der * (x[i + k] - x[j + k]);
          grad[j + k] += der * (x[j + k] - x[i + k]);          
        }*/
        for(int k = 0; k < d; k ++) {
          double t;
          t = Y[k] * der;
          grad[i + k] += t;
          grad[j + k] -= t;          
        }
              
      }
    }
    for(int i = 0; i < n; i ++)
      grad[i] *= 2.;    
    return r;
  }
  
  
  
  /**
   * Evaluate Hessian matrix
   * @param n number of atoms
   * @param d space dimensionality
   * @param x start point
   * @param H resulting Hessian
   */
  void hessian(int N, int D, const FT* x, FT* H)
  {
    FT cell, q, d1, d2;
    int n;
    n = N * D;
    
    for(int i = 0; i < N; i ++){
      for(int j = i + 1; j < N; j ++) {
        int I, J;
        I = i * D;
        J = j * D;
        q = 0.;
        for(int k = 0; k < D; k ++)
          q += BNBSQR(x[I + k] - x[J + k]);
        d1 = df(q);
        d2 = ddf(q);
        for(int k = 0; k < D; k ++) {
          cell = - (2. * d1 + 4 * BNBSQR(x[i * D + k] - x[j * D + k]) * d2);
          I = i * D + k;
          J = j * D + k;
          H[I * (n + 1) + J] = cell;
          H[J * (n + 1) + I] = cell;
        }
        for(int k = 0; k < D; k ++) {
          for(int l = k + 1; l < D; l ++) {
            cell = - 4. * (x[i * D + k] - x[j * D + k]) * (x[i * D + l] - x[j * D + l]) * d2;
            I = i * D + k;
            J = j * D + l;
            H[I * (n + 1) + J] = cell;                
            H[J * (n + 1) + I] = cell;
            I = i * D + l;
            J = j * D + k;
            H[I * (n + 1) + J] = cell;                
            H[J * (n + 1) + I] = cell;
          }
        }                     
      }
    }
    for(int i = 0; i < N; i ++) {
      int I, J;
      for(int k = 0; k < D; k ++) {
        for(int l = 0; l < D; l ++) {
          I = i * D + k;
          J = i * D + l;
          H[I * (n + 1) + J] = 0;
          for(int m = 0; m < N; m ++) {
            if(m != i) {
              int M;
              M = m * D + l;
              H[I * (n + 1) + J] -= H[I * (n + 1) + M];
            }
          }
        }
      }
    }
  }
  
 /**
  * Retrieve argument of maximum for 1st derivation
  * @return argument value
  */
  virtual FT dfmax() = 0;

 /**
   * Retrieve argument of maximum for 2nd derivation
   * @return argument value
  */
  virtual FT ddfmax() = 0;

  protected:
  
/**
 * The potential function
 *
 * @param dsq squared distance
 *
 * @return the value of potential
 *
 */
  virtual FT potent(FT dsq) = 0;

/**
 * First derivative for potent
 *
 * @param dsq squared distance
 *
 * @return the value of the first derivative
 */
  virtual FT df(FT dsq) = 0;

  
  /**
   * Second derivative
   * @param dsq squared distance
   *
   * @return the value of the derivative
   */
  virtual FT ddf(FT dsq) = 0;
 
  /**
   * Derivative and value
   * @param dsq square distance
   * @param fval function value
   * @param der derivative value
   */
  virtual void fandf(FT dsq, FT* fval, FT* der)
  {
    BNB_ERROR_REPORT("fandf not implemented\n");
  }
   
  /**
   * Retrieve left bound for linear interpolation
   * @return left bound
   */
  virtual FT leftbound() = 0;
  

  FT mScale;
  FT mA;
  FT mB;
  FT mDelta;
};

#endif
