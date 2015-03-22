#include <stdlib.h>
#include "jacobi.hpp"
#include <util/common/vec.hpp>
#include <util/common/matr.hpp>

#define N 8

//double A[N][N] = {{1, -1, -3}, {-1, 1, -3}, {-3, -3, -3}};
//double A[N][N] = {{0, 1, 1}, {1, 0, 1}, {1, 1, 0}};
//double A[N][N] = {{5, 4, 2}, {4, 5, 4}, {2, 4, 5}};
double A[N][N] = {{5, 3, 1},
                  {3, 5, 4},
                  {1, 4, 5}};

static double d[N], b[N], c[N], x[N];

static double V[N][N], Vt[N][N], B[N][N], C[N][N], D[N][N];

static void init()
{
  for(int i = 0; i < N; i ++) {
    A[i][i] = 5.;
    b[i] = 1.;
    for(int j = i + 1; j < N; j ++) {
      A[i][j] = A[j][i] = 5 * (double)rand()/ (double)RAND_MAX;
    }    
  }
}

main()
{
  Jacobi<double> jac;
  
  for(int I = 0; I < 5; I ++) {
    init();
    jac.solve(N, (double*)A, (double*)d, (double*)V);
    jac.sort(N, (double*)d, (double*)V);
    for(int i = 0; i < N - 1; i ++) 
      for(int j = i + 1; j < N; j ++)
        A[i][j] = A[j][i];
    
    for(int k = 0; k < N; k ++) {
      double x[N];
      for(int i = 0; i < N; i ++) {
        x[i] = 0.;
        for(int j = 0; j < N; j ++) {
          x[i] += A[i][j] * V[j][k];
        }
      }
      for(int i = 0; i < N; i ++) {
        x[i] -= d[k] * V[i][k];
      }
      double norm;
      norm = VecUtils::vecNormTwo(N, x);
      VecUtils::vecPrint(N, x);
      printf("%d eigenvalue %lf: %lf\n", k, d[k], norm);
    }
      /*
    for(int i = 0; i < N; i ++){
      D[i][i] = d[i];
    }
    MatrUtils::trans(N, (double*)V, (double*)Vt);
    MatrUtils::mxv(N, N, (double*)Vt, (double*)b, (double*)c);
    for(int i = 0; i < N; i ++)
      c[i] /= d[i];
    MatrUtils::mxv(N, N, (double*)V, (double*)c, (double*)x);
    */
    jac.solveSys(N, (double*)d, (double*)V, (double*)b, (double*) x);
    MatrUtils::mxv(N, N, (double*)A, (double*)x, (double*)c);
    
    VecUtils::vecPrint(N, b);
    VecUtils::vecPrint(N, c);
    printf("\n");    
  }
}
