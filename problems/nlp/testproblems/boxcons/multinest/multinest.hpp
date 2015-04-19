/* 
 * File:   multinest.hpp
 * Author: medved
 *
 * Created on February 11, 2015, 8:08 PM
 */

#ifndef MULTINEST_HPP
#define	MULTINEST_HPP

#include <problems/optlib/objective.hpp>

/**
 * The function is a sum of a type
 * 
 *  sum c[i] * (x[i]^4 - 5 x[i]^2 + 4)
 *  where c is a vector of non-negative coefficients
 */

class MultiNestObj : public Objective <double> {
public:

    /**
     * Constructor
     * 
     * @param n dimension
     * @coeff vector of coefficients
     */
    MultiNestObj(int n, SmartArrayPtr<double>& coeff) {
        Objective<double>::setDim(n);
        mCoeff = coeff;
    }

    double func(const double* x) {
        int n = Objective<double>::mDim;
        double v = 0;
        for (int i = 0; i < n; i++) {
            v += mCoeff[i] * (x[i] * x[i] * x[i] * x[i] - 5 * x[i] * x[i] + 4);
        }
        return v;
    }

    void grad(const double* x, double* g) {
        int n = Objective<double>::mDim;
        for (int i = 0; i < n; i++) {
            g[i] = mCoeff[i] * (4 * x[i] * x[i] * x[i] - 10 * x[i]);
        }
    }

private:

    SmartArrayPtr<double> mCoeff;
};


#endif	/* MULTINEST_HPP */

