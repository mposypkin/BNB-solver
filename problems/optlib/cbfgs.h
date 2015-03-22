#ifndef __CBFGS_H__
#define __CBFGS_H__

#ifdef __cplusplus
extern "C" void cbfgs(int n,int m,double x[],double f,double g[],int diagco,double diag[],int iprint[2],double eps,double xtol,double w[],int *iflag,double dguess);
extern "C" void cbfgsInit();
#else 
extern  void cbfgs(int n,int m,double x[],double f,double g[],int diagco,double diag[],int iprint[2],double eps,double xtol,double w[],int *iflag,double dguess);
extern  void cbfgsInit();
#endif


#endif

