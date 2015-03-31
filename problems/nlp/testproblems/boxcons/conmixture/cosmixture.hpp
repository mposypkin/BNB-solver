/* 
 * File:   cosmixture.hpp
 * Author: medved
 *
 * Consin mixture function
 * 
 * Created on March 23, 2015, 10:14 PM
 */

#ifndef COSMIXTURE_HPP
#define	COSMIXTURE_HPP

#include <math.h>
#include <problems/optlib/objective.hpp>

/**
 * The function is 
 * 
 *  sum x_i^2 - a * sum cos(b x_i)
 *  
 */

class CosMixture : public Objective <double> {
public:

    /**
     * Constructor 
     * @param n dimension
     * @param a 1st coefficient
     * @param b 2nd coefficient
     */
    CosMixture(int n, double a = 0.1, double b = 5 * M_PI) : mA(a), mB(b) {
        Objective<double>::setDim(n);
    }

    double func(const double* x) {
        int n = Objective<double>::mDim;
        double v = 0;
        for (int i = 0; i < n; i++) {
            v += x[i] * x[i] - mA * cos(mB * x[i]);
        }
        return v;
    }

    void grad(const double* x, double* g) {
        int n = Objective<double>::mDim;
        for (int i = 0; i < n; i++) {
            g[i] = 2 * x[i] + mA * mB * sin(mB * x[i]);
        }
    }

private:

    double mA;

    double mB;
};



#endif	/* COSMIXTURE_HPP */

