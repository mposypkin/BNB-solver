#ifndef __MATR_HPP__
#define __MATR_HPP__

#include "vec.hpp"

 class MatrUtils {
  public:
    
    /**
     * Matrix vector multiply
     * @param m number of rows in a matrix
     * @param n number of columns in a matrix
     * @param A matrix 
     * @param x mutiplier
     * @param y result
     */
    template <typename FT> static void mxv(int m, int n, FT* A, FT* x, FT* y)
    {
      for(int i = 0; i < m; i ++) {
        y[i] = VecUtils::vecScalarMult(n, A + i * n, x);
      }
    }
      
    
    /**
     * Square matrix multiply
     * @param m matrix dimension
     * @param A first operand
     * @param B second operand
     * @param C result
     */
    template <typename FT> static void mxmq(int m, FT* A, FT* B, FT* C)
    {
      for(int i = 0; i < m; i ++) {
        for(int j = 0; j < m; j ++) {
          FT v;
          v = 0.;
          for(int k = 0; k < m; k ++) 
            v += A[i * m + k] * B[k * m + j];
          C[i * m + j] = v;
        }
      }
    }
    
    /**
     * Matrix transposition
     * @param m number of dimensions
     * @param A source
     * @param B result
     */
    template <typename FT> static void trans(int m, FT* A, FT* B)
    {
      for(int i = 0; i < m; i ++) {
        for(int j = 0; j < m; j ++) 
          B[j * m + i] = A[i * m + j];
      }
    }
    
    /**
     * Print m x n matrix
     * @param m number  of rows
     * @param n number of colums
     * @param A matrix
     */
    template <typename FT> static void print(int m, int n, FT* A)
    {
      for(int i = 0; i < m; i ++) {
        for(int j = 0; j < n; j ++)
          printf("%lf ", A[i * m + j]);
        printf("\n");
      }
    }
};
#endif
