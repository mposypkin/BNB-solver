#include "simpgrad.hpp"
#include "newton.hpp"
#include "mnewton.hpp"
#include "clevgrad.hpp"
#include "steepdesc.hpp"
#include "goldls.hpp"
#include "fletchsearch.hpp"
#include "newtonls.hpp"
#include "conjgrad.hpp"
#include "lmbfgs.hpp"
#include "mbfgs.hpp"
#include "apxobjective.hpp"

class Booth : public Objective <double>
{
  public:
    
    Booth()
    {
      Objective<double>::setDim(2);
    }
  
    double func(const double* vec) 
    {
      double x1, x2;
      x1 = vec[0];
      x2 = vec[1];
      return BNBSQR(x1 + 2 * x2 - 7) + BNBSQR(2 * x1 + x2 - 5);
    }
  
    void grad(const double* vec, double* g)
    {
      double x1, x2;
      x1 = vec[0];
      x2 = vec[1];
      g[0] = 10 * x1 + 8 * x2 - 34;
      g[1] = 8* x1 + 10 * x2 - 38;
    }
  
    void hess(const double* vec, double* H)
    {
      H[0] = 10;
      H[1] = H[3] = 8;
      H[4] = 10;
    }
  
};


class BoothBB : public ApxObjective <double>
{
  public:

    BoothBB() : ApxObjective <double>()
    {
      Objective<double>::setDim(2);
    }

    double func(const double* vec)
    {
      double x1, x2;
      x1 = vec[0];
      x2 = vec[1];
      return BNBSQR(x1 + 2 * x2 - 7) + BNBSQR(2 * x1 + x2 - 5);
    }
};

main()
{
  Booth bt;
  BoothBB btbb;
  double x0[2] = {20, 30};
  double x[2];
  double v;
  bool rv;
  double pen;
  for(int i = 0; i < 2; i ++)
    x[i] = x0[i];
  FletchSearch<double> ls;
  for(int i = 0; i < 2; i ++)
    x[i] = x0[i];
  ConjGrad<double> fr;
  fr.setLineSearch(&ls);
  fr.setObjective(&bt);
  fr.setMaxIters(10000);
  fr.setEps(0.00001);
  rv = fr.search(x, &v);

  if(rv){
      pen = v * v;
      pen += BNBSQR(x[0] - 1.);
      pen += BNBSQR(x[1] - 3.);
      if(pen > 0.001)
      BNB_ERROR_REPORT("FR failed");
  }  else {
      BNB_ERROR_REPORT("FR failed");
  }


  for(int i = 0; i < 2; i ++)
    x[i] = x0[i];
  LMBFGS lm;
  lm.setObjective(&bt);
  lm.setEps(0.00001);
  rv = lm.search(x, &v);
  if(rv){
      pen = v * v;
      pen += BNBSQR(x[0] - 1.);
      pen += BNBSQR(x[1] - 3.);
      if(pen > 0.001)
      BNB_ERROR_REPORT("LBFGS failed");
  }  else {
      BNB_ERROR_REPORT("LBFGS failed");
  }
  
  for(int i = 0; i < 2; i ++)
    x[i] = x0[i];
  MBFGS mm;
  mm.setObjective(&bt);
  mm.setEps(0.00001);
  rv = mm.search(x, &v);
  if(rv){
      pen = v * v;
      pen += BNBSQR(x[0] - 1.);
      pen += BNBSQR(x[1] - 3.);
      if(pen > 0.001)
      BNB_ERROR_REPORT("MBFGS failed");
  }  else {
      BNB_ERROR_REPORT("MBFGS failed");
  }


  for(int i = 0; i < 2; i ++)
      x[i] = x0[i];
  MBFGS mm1;
  mm1.setObjective(&btbb);
  mm1.setEps(0.00001);
  btbb.setH(0.0000001);
  rv = mm1.search(x, &v);
  if(rv){
      pen = v * v;
      pen += BNBSQR(x[0] - 1.);
      pen += BNBSQR(x[1] - 3.);
      if(pen > 0.001)
      BNB_ERROR_REPORT("MBFGS failed for black-box");
  }  else {
      BNB_ERROR_REPORT("MBFGS failed for black-box");
  }
}

