#ifndef __SULTAN_HPP__
#define __SULTAN_HPP__

#include "rnabase.hpp"

void initSultan(int& n, RNABase<double> & rna)
{
  double *diag, *sdiag, *kap;
  n = 3;
  diag = (double*) calloc(n, sizeof(double));
  sdiag = (double*) calloc(n, sizeof(double));
  kap = (double*) calloc(n, sizeof(double));

  sdiag[0] = 1.;
  sdiag[1] = 1.;
  kap[0] = 6.;
  kap[1] = 5.1;
  kap[2] = 3.9;      
  rna.setDim(n);
  rna.setDiag(diag);
  rna.setSDiag(sdiag);
  rna.setKap(kap);
}

void initSultan2(int& n, RNABase<double> & rna)
{
  double *diag, *sdiag, *kap;
  n = 5;
  
  diag = (double*) calloc(n, sizeof(double));
  sdiag = (double*) calloc(n, sizeof(double));
  kap = (double*) calloc(n, sizeof(double));

  for(int i = 0; i < 5; i ++) {
        sdiag[i] = 1.;
  }
  kap[0] = 24.;
  kap[1] = 27.;
  kap[2] = 33.;      
  kap[3] = 37.;      
  kap[4] = 24.;
  rna.setDim(n);
  rna.setDiag(diag);
  rna.setSDiag(sdiag);
  rna.setKap(kap);
}
#endif
