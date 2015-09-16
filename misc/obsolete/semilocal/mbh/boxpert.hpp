#ifndef __BOXPERT_HPP__
#define __BOXPERT_HPP__

#include <stdlib.h>
#include <time.h>
#include "pertruber.hpp"
#include <util/common/bnberrcheck.hpp>

template <typename FT> class BoxPertruber :
  public Pertruber < FT > 
{
  public:
    
    /**
     * Constructor
     * 
     * @param ampl amplitude
     * @param n number of starts
     */    
    BoxPertruber(FT ampl = 0., int n = 0.) {
      mAmpl = ampl;
      mN = n;
      mNOS = 0;
      mGen = NULL;
      srand(time(NULL));
    }
    
    /**
     * Setup number of starts
     * @param n number of starts
     */
    void pregenerate(int n)
    {
      BNB_ASSERT(mN);
      if(n != mNOS) {
        mNOS = n;
        if(mGen != NULL) 
          free(mGen);        
        mGen = (FT*) malloc(mNOS * mN * sizeof(FT));
        BNB_ASSERT(mGen);
      }
      for(int i = 0; i < mNOS; i ++) {
        FT* y;
        y = mGen + i * mN;
        for(int j = 0; j < mN; j ++) {
          FT alpha;
          alpha = mAmpl * (0.5 - ((double)rand() / (double)RAND_MAX));
          y[j] = alpha;
        }              
      }
    }
    
    void pertrub(int cnt, FT *x, FT *y){
      BNB_ASSERT(mN);
      FT* z;
      
      if(mGen) {
         z = mGen + cnt * mN;
      }
      
      for(int i = 0; i < mN; i ++) {
        FT alpha;
        if(mGen) 
          alpha = z[i];
        else {
          //double amp = mAmpl * (1. - 0.7 * ((double)rand() / (double)RAND_MAX));
          alpha = mAmpl  * (0.5 - ((double)rand() / (double)RAND_MAX));
        }
        y[i] = x[i] + alpha;
      }
    }
    
    FT* getGen()
    {
      return mGen;
    }
    
    void setGen(FT* gen) {
      if(mGen != NULL) {
        free(mGen);
      }
      mGen = gen;
    }
    
    void setAmpl(FT ampl){
      mAmpl = ampl;
    }
  private:
    
    FT mAmpl;
    int mN;
    int mNOS;        
    FT* mGen;
};

#endif
