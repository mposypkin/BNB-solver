#ifndef _OPTLIB_HPP_
#define _OPTLIB_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file optlib.hpp
 */


#include <stdio.h>
#include <queue>
#include <util/common/smartptr.hpp>

/**
 * Solve 1-dim linear programming problem using Danzig method
 *
 * @param n dimension
 *
 * @param r maximal "wheight" (right part of the inequality)
 *
 * @param A array of "wheights"
 *
 * @param C array of "prices"
 *
 * @param X variable values (1,0 or -1 if undefined)
 *
 * @param a sum of wheights of fixed variables
 *
 * @param c sum of prices of fixed variables
 *
 * @param estim sxtimation 
 *
 * @param bn variable for brnaching (-1 if the solution is precise)
 *
 * @return 
 *
 */
int danzig (int n, int r, const int *A, const int *C, int *X, int *w, int *p,
	     int *estim, int *bn)
{
  int i;
  int j;
  int f;
  int a;
  int rt;
  SmartArrayPtr<int> y;

  y.realloc(n);
  if (w == NULL)
    for (a = 0, j = 0; j < n; j++)
      a += ((X[j] == 1) ? A[j] : 0);
  else
    a = *w;

  if (a > r)
    rt = 0;
  else
  {
    if (p == NULL)
      for (f = 0, j = 0; j < n; j++)
	f += ((X[j] == 1) ? C[j] : 0);
    else
      f = *p;
    i = 0;
    if (a < r)
      for (; i < n; i++)
      {
	if (X[i] == -1)
	{
	  a += A[i];
	  f += C[i];
	  if (a < r)
	  {
	    y[i] = 1;
	    continue;
	  }
	  else if (a == r)
	  {
	    y[i] = 1;
	    i++;
	    break;
	  }
	  else
	  {
	    break;
	  }
	}
	else
	  y[i] = X[i];
      }

    if (a <= r)
    {
      for (j = 0; j < i; j++)
	X[j] = y[j];
      *bn = -1;
      if (w != NULL)
	*w = a;
      if (p != NULL)
	*p = f;
      *estim = f;
    }
    else
    {
      double fold = f;
      double aold = a;
      double ci = C[i];
      double ai = A[i];
      double R = r;

      *estim = (int) (fold + ci * (R - aold) / ai);
      *bn = i;
    }
    rt = 1;
  }

  return rt;
}




const int MIN = 0;
const int MAX = 1;

/**
 * Finds maximum (minimum) in the array of n elements
 *
 * @param n number of elements in the array
 *
 * @param x array
 *
 * @param ind index of the maximal element
 *
 * @return 
 *
 */
template < class T, int what > T
findextremum (int n, T * x, int *ind)
{
  T e;
  int j;

  j = 0;
  e = x[j];

  if (what == 0)
    for (int i = 0; i < n; i++)
      {
	if (x[i] < e)
	  {
	    e = x[i];
	    j = i;
	  }
      }
  else
    for (int i = 0; i < n; i++)
      {
	if (x[i] > e)
	  {
	    e = x[i];
	    j = i;
	  }
      }


  *ind = j;
  return e;
}


/**
 * Auxilarly class for lambdaSort function
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 */
template < class T > class Triple
{
public:
  bool operator < (const Triple & t) const
  {
    return l < t.l;
  }

  T a;
  T c;
  int i;
  double l;

};


/**
 * Sorts a and c arrays according to decreasing c[i]/a[i]
 *
 * @param n number of elements in arrays
 *
 * @param a array a
 *
 * @param c array c
 *
 * @param s2n source to sorted index translation
 *
 * @param n2s sorted to source index translation
 *
 */
template < class T > void
lambdaSort (int n, T * a, T * c, T * s2n, T * n2s)
{
  int i;
  std::priority_queue < Triple < T > >q;

  for (i = 0; i < n; i++)
    {
      Triple < T > t;
      t.a = a[i];
      t.c = c[i];
      t.i = i;
      t.l = (double) (c[i]) / (double) (a[i]);

      q.push (t);
    }

  i = 0;
  while (!q.empty ())
    {
      Triple < T > t;

      t = q.top ();
      a[i] = t.a;
      c[i] = t.c;
      s2n[t.i] = i;
      n2s[i] = t.i;
      q.pop ();
      i++;
    }
}

#endif
