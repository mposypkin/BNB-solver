#ifndef __SULT_HPP__
#define __SULT_HPP__

#include "dna.hpp"

DNA* initSultan3(int& m, int& n)
{
  SmartArrayPtr<double> *diag, *sdiag, *kap;
  m = 3;
  n = 4;
  
  DNA* dna = new DNA(n, n - m);
  diag = dna->getDiag();
  sdiag = dna->getSDiag();
  kap = dna->getKap();

  diag->alloc(3);
  sdiag->alloc(3);
  kap->alloc(3);
  (*diag)[0] = 0.;
  (*diag)[1] = 0.;
  (*diag)[2] = 0.;
  (*sdiag)[0] = 1.;
  (*sdiag)[1] = 1.;
  (*kap)[0] = 6.;
  (*kap)[1] = 5.1;
  (*kap)[2] = 3.9;      
  return dna;
 }

 DNA* initSultan5(int& m, int& n)
 {
   SmartArrayPtr<double> *diag, *sdiag, *kap;
   m = 5;
   n = 6;
  
   DNA* dna = new DNA(n, n - m);
   diag = dna->getDiag();
   sdiag = dna->getSDiag();
   kap = dna->getKap();

   diag->alloc(m);
   sdiag->alloc(m);
   kap->alloc(m);
   
   for(int i = 0; i < 5; i ++) {
     (*diag)[i] = 0.;
     (*sdiag)[i] = 1.;
   }
   (*kap)[0] = 24.;
   (*kap)[1] = 27.;
   (*kap)[2] = 33.;      
   (*kap)[3] = 37.;      
   (*kap)[4] = 24.;

   return dna;
 }
 
#endif
