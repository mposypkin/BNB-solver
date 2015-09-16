#ifndef __MOLEC_UTILS__
#define __MOLEC_UTILS__

#include <stdio.h>
#include <math.h>
#include <util/common/util.hpp>
#include <problems/optlib/projector.hpp>



/**
 * Evaluates the square  distance between centers of two rectangles 
 *
 * @param spaced the space dimensionality
 * 
 * @param li first rec lower bounds 
 *
 * @param ui first rect upper bounds 
 *
 * @param lj second rect lower bounds
 *
 * @param uj second rect upper bounds
 *
 * @return square distance between centers
 *
 */
   template <typename FloatType> FloatType evalSqrCntDistance(int spaced, const FloatType* li, const FloatType* ui, const FloatType* lj, const FloatType* uj)
{
  FloatType d = 0., a, b;

  for(int k = 0; k < spaced; k ++) {
    a = 0.5 * (ui[k] + li[k]);
    b = 0.5 * (uj[k] + lj[k]);
    d += BNBSQR(b - a) ;
  }    
  return d;
}
  
 

/**
 * Evaluates minimum of the square  distance between points two parallelepipeds 
 *
 * @param spaced the space dimensionality
 * 
 * @param li first rec lower bounds 
 *
 * @param ui first rect upper bounds 
 *
 * @param lj second rect lower bounds
 *
 * @param uj second rect upper bounds
 *
 * @return square distance between centers
 *
 */
  template <typename FloatType> FloatType evalSqrMinDistance(int spaced, const FloatType* li, const FloatType* ui, const FloatType* lj, const FloatType* uj)
{
  FloatType d = 0.;

  for(int k = 0; k < spaced; k ++) 
    d += BNBSQR(BNBMAX(BNBMAX(lj[k] - ui[k], li[k] - uj[k]), 0)) ;
    
  return d;
}


/**
 * Evaluates maximum of the square  distance between points two parallelepipeds 
 *
 * @param spaced the space dimensionality
 * 
 * @param li first rec lower bounds 
 *
 * @param ui first rect upper bounds 
 *
 * @param lj second rect lower bounds
 *
 * @param uj second rect upper bounds
 *
 * @return square distance between centers
 *
 */

  template <typename FloatType> FloatType evalSqrMaxDistance(int spaced, const FloatType* li, const FloatType* ui, const FloatType* lj, const FloatType* uj)
{
  FloatType d = 0., a, b;
    
  for(int k = 0; k < spaced; k ++) {
    a = BNBMIN(li[k], lj[k]);
    b = BNBMAX(ui[k], uj[k]);
    d += BNBSQR(b - a);
  }
  return d;
}

template <typename FT> class Molviz {
  
  public:
    
    static void printDist(int natoms, int ndims, FT* x, FILE* f = stdout)
    {
      for(int i = 0; i < natoms; i ++) {
        for(int j = i + 1; j < natoms; j ++) {
          FT q,d;
          q = 0.;
          for(int k = 0; k < ndims; k ++)
            q += BNBSQR(x[i * ndims + k] - x[j * ndims + k]);
          d = sqrt(q);
          fprintf(f, "d[%d][%d] = %lf\n", i, j, d);
        }
      }
    }
      
    static void printGnuplot(int natoms, int ndims, FT* x, FILE* f = stdout) {
      for(int i = 0; i < natoms; i ++) {
        for(int j = 0; j < ndims; j ++)
          fprintf(f, "%lf ", x[i * ndims + j]);
        fprintf(f, "\n");
      }
    }
    
    

};

/**
 * Perform projection to a given set of coordinates
 */
template <typename FT> class MolecProjector : public Projector <FT> {
  public:
    /**
     * Constructor. 
     * @param ndims number of space dimensions
     */
    MolecProjector(int ndims) 
    {
      mNDims = ndims;
    }
    
    void project(int n, FT* x)
    {
      int k, ind;
      k = 0;
      ind = 0;
      for(int i = mNDims; i >=1; i --) {
        for(int j = 0; j < i; j ++) {
          ind = k * mNDims + j;
          if(ind >= n)
            break;
          x[ind] = 0;
        }
        if(ind >= n)
          break;
        k ++;
      }
    }
    
  private:
    int mNDims;
};


#endif
