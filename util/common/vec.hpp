#ifndef _VEC_HPP_
#define _VEC_HPP_
/**
 * Subroutines for frequently used vector operations
 *
 * @author Mikhail Posypkin, ISA RAS, posypkin@isa.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file vec.hpp
 */

#include <math.h>
#include <iostream>
#include "util.hpp"

class VecUtils {
public:

    template<class T> static void vecSet(int n, const T x, T* y) {
        for (int i = 0; i < n; i++)
            y[i] = x;
    }

    template<class T> static T vecNormOne(int n, const T* x) {
        T v = 0.;
        for (int i = 0; i < n; i++)
            v = BNBMAX(BNBABS(x[i]), v);
        return v;
    }

    template<class T> static T vecScalarMult(int n, const T* x, const T* y) {
        T v;
        v = 0;
        for (int i = 0; i < n; i++)
            v += x[i] * y[i];
        return v;
    }

    template<class T> static T vecNormTwoSqr(int n, const T* x) {
        T v = 0.;
        for (int i = 0; i < n; i++)
            v += BNBSQR(x[i]);
        return v;
    }

    template<class T> static T vecNormTwo(int n, const T* x) {
        T v = 0.;
        for (int i = 0; i < n; i++)
            v += BNBSQR(x[i]);
        return sqrt(v);
    }

    template<class T> static T vecDist(int n, const T* x, const T* y) {
        T v = 0.;
        for (int i = 0; i < n; i++)
            v += BNBSQR(y[i] - x[i]);
        return sqrt(v);
    }

    template<class T> static T vecDistAbs(int n, const T* x, const T* y) {
        T v = 0.;
        for (int i = 0; i < n; i++) {
            T u = BNBABS(y[i] - x[i]);
            v = BNBMAX(u, v);
        }
        return v;
    }

    template<class T> static T vecCenter(int n, const T* x, const T* y, T* z) {
        T v = 0.;
        for (int i = 0; i < n; i++)
            z[i] = 0.5 * (x[i] + y[i]);
        return sqrt(v);
    }

    template <class T> static void vecCopy(int n, const T * x, T* y) {
        for (int i = 0; i < n; i++)
            y[i] = x[i];
    }

    template <class T> static void revert(int n, T * x) {
        for (int i = 0; i < n; i++)
            x[i] *= -1;
    }

    template <class T> static void vecMult(int n, const T * x, T alpha, T* y) {
        for (int i = 0; i < n; i++)
            y[i] = alpha * x[i];
    }

    template <class T> static void vecSaxpy(int n, const T * x, const T *y, T alpha, T* z) {
        for (int i = 0; i < n; i++)
            z[i] = x[i] + y[i] * alpha;
    }

    template <class T> static T vecDotProd(int n, const T * x, const T *y) {
        T z = 0;
        for (int i = 0; i < n; i++)
            z += x[i] * y[i];
        return z;
    }

    
    template <class T> static void vecRotate(int n, const T * M, const T * x, T* result){
	for(int j=0; j<n; ++j){
	    result[j] = vecDotProd(n, M+j*n, x);
	}
    }
    
    template<class T> static void vecPrint(int n, const T* x) {
        std::cout << "[";
        for (int i = 0; i < n; i++) {
            std::cout << x[i];
            if (i != (n - 1))
                std::cout << ",";
        }
        std::cout << "]\n";
    }

    template<class T> static T max(int n, const T *x, int *pos) {
        T mv = x[0];
        int p = 0;
        for (int i = 0; i < n; i++) {
            if (x[i] > mv) {
                p = i;
                mv = x[i];
            }
        }
        if (pos)
            *pos = p;
        return mv;
    }

    template<class T> static T min(int n, const T *x, int *pos) {
        T mv = x[0];
        int p = 0;
        for (int i = 0; i < n; i++) {
            if (x[i] < mv) {
                p = i;
                mv = x[i];
            }
        }
        if (pos)
            *pos = p;
        return mv;
    }

    template<class T> static T maxAbs(int n, const T *x, int *pos) {
        T mv = BNBABS(x[0]);
        int p = 0;
        for (int i = 0; i < n; i++) {
            if (BNBABS(x[i]) > mv) {
                p = i;
                mv = BNBABS(x[i]);
            }
        }
        if (pos)
            *pos = p;
        return mv;
    }

    template<class T> static T minAbs(int n, const T *x, int *pos) {
        T mv = BNBABS(x[0]);
        int p = 0;
        for (int i = 0; i < n; i++) {
            if (BNBABS(x[i]) < mv) {
                *pos = i;
                mv = BNBABS(x[i]);
            }
        }
        if (pos)
            *pos = p;
        return mv;
    }
    
    /**
     * Projector to the hyperplane
     * @param n dimension
     * @param nplane plane normal vector
     * @param sourcev vector to project
     * @param resultv projected vector     
     */
    template<class T> static void project(int n, const T* nplane, const T* sourcev, T* resultv) {
        T s = vecDotProd(n, nplane, sourcev);
        T csq = vecNormTwoSqr(n, nplane);
        T a = - s / csq;
        vecSaxpy(n, sourcev, nplane, a, resultv);
    }

    /**
     * Subroutine for finding determinant of a matrix
     *
     * @author Anton Rubtsov, ISA RAS, rubtsov493@gmail.com
     * 
     */

    template <class T> static T det(int size, const T *m) {
        if (size == 1) {
            return *m;
        }
        if (size == 4) {
            return m[0] * m[3] - m[1] * m[2];
        } else {
            T res = 0.0;
            int m_size = (int) sqrt(size);

            int minor_size = (m_size - 1)*(m_size - 1);
            T * minor = new T [minor_size];

            for (int i = 0; i < m_size; ++i) {
                int count = 0;

                for (int j = m_size; j < size; ++j) {
                    if (!(j % m_size == i)) {
                        minor[count] = m[j];
                        count++;
                    }
                }

                if (i % 2 == 0)
                    res += m[i] * det(minor_size, minor);
                else
                    res -= m[i] * det(minor_size, minor);
            };
            //~ cout <<"Size: " << m_size <<", Res: "<< res << endl;
            delete[] minor;
            return res;
        };
    }
};
#endif
