#include "gauss.hpp"

#define N 3

main(int argc, char* argv[])
{
  double A[N][N + 1] = {
    {10, -7, 0, 7},
    {-3, 2.099, 6, 3.901},
    {5, -1, 5, 6}};
    double B[N][N + 1] = {
      {10, -7, 0, 7},
      {-3, 2.099, 6, 3.901},
      {5, -1, 5, 6}};;
  double x[N];
  Gauss<double> gauss;
  gauss.solve(N, (double*)A, x);
  for(int i = 0; i < N; i ++){
    for(int j = 0; j < N + 1; j ++) {
      printf("\t%f", A[i][j]);
    }
    printf("\n");
  }    
  printf("\n x = ");
  for(int i = 0; i < N; i ++)
    printf("\t%lf", x[i]);
  printf("\n");
  double amax = 0;
  for(int k = 0; k < N; k ++) {
    double a = 0;
    for(int l = 0; l < N; l ++) 
      a += x[l] * B[k][l];
    amax = BNBMAX(BNBABS(a - B[k][N]), amax);
  }
  printf("Nash: diff = %lf\n", amax);
  printf("====");
  for(int i = 0; i < 10; i ++) {
    for(int k = 0; k < N; k ++)
      for(int l = 0; l < N + 1; l ++)
        B[k][l] = A[k][l] = 5 - 10 * ((double)rand()) / (double)RAND_MAX;
    gauss.solve(N, (double*)B, x);
    double amax = 0;
    for(int k = 0; k < N; k ++) {
      double a = 0;
      for(int l = 0; l < N; l ++) 
        a += x[l] * B[k][l];
      amax = BNBMAX(BNBABS(a - B[k][N]), amax);
    }
    printf("%d: diff = %lf\n", i, amax);
  }
}
