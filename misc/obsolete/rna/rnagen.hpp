#ifndef __RNAGEN_HPP__
#define __RNAGEN_HPP__

#include <string.h>
#include <util/common/bnberrcheck.hpp>
#include "rnabase.hpp"
#include <util/common/vec.hpp>

extern double DNA_TRANSITION_MATRIX[4][4];
extern double DNA_OXIDATION_VECTOR[4];

/**
 * Generate a matrix based on RNA syntax
 */
class RnaGenerator {
  public:
    
    /**
     * Generate an initial data from a string rpresentation of DNA
     * @param str string representation (in lowercase) terminated by '\0'
     * @param  n dimension (output)
     * @param rna structure to fill (output)
     */
    static void generate(const char* str, int& n, RNABase<double>& rna)
    {
      double *diag, *sdiag, *kap;
      n = strlen(str);
      diag = (double*) calloc(n, sizeof(double));
      sdiag = (double*) calloc(n, sizeof(double));
      kap = (double*) calloc(n, sizeof(double));

      for(int i = 0; i < n; i ++) {
        int codei;
        codei = letCode(str[i]);
        diag[i] = DNA_OXIDATION_VECTOR[codei];
        if(i != n - 1) {
          int j, codej;
          double v;
          j = i + 1;
          codej = letCode(str[j]);
          v = 15.19 * DNA_TRANSITION_MATRIX[codei][codej];
          sdiag[i] = mScale * v;
        }
      }
      VecUtils::vecPrint(n - 1, sdiag);
      for(int i = 0; i < n; i ++)
        kap[i] = mScale * 4.;      
      rna.setDim(n);
      rna.setDiag(diag);
      rna.setSDiag(sdiag);
      rna.setKap(kap);
    }
    
    static void setScale(double scale)
    {
      mScale = scale;
    }
    
  private:
    static int letCode(char let)
    {
      int code;
      switch(let) 
      {
        case 'a':
          code = 0;
          break;
        case 'c':
          code = 1;
          break;
        case 't':
          code = 2;
          break;
        case 'g':
          code = 3;
          break;
        default:
          fprintf(stderr, "==>> %c <== ", let);
          BNB_ERROR_REPORT("Undefined letter in a DNA sequence\n");          
      }
      return code;
    }
    
    static double mScale;
    
};

#endif
