/* 
 * File:   rosenbrok.hpp
 * Author: medved
 *
 * Created on February 11, 2015, 10:20 PM
 */

#ifndef ROSENBROK_HPP
#define	ROSENBROK_HPP
#include <util/common/vec.hpp>
#include <util/common/interval.hpp>

/**
 * Rosenbrok function
 */
class Rosenbrok : public Objective<double> {
public:

    /**
     * Constructor
     * @param n dimension
     */
    Rosenbrok(int n) {
        mDim = n;
    }

    double func(const double* x) {
        double v;
        int n;
        v = 0.;
        n = getDim();
        for (int i = 0; i < (n - 1); i++) {
            v += 100. * BNBSQR(x[i] * x[i] - x[i + 1]) + BNBSQR(x[i] - 1);
        }
        return v;
    }

    void grad(const double* x, double* g) {
        int n;
        n = getDim();
        for (int i = 0; i < n; i++) {
            double v;
            v = 0.;
            if (i != 0)
                v += 200. * (x[i] - x[i - 1] * x[i - 1]);
            if (i != (n - 1))
                v += 400. * x[i] * x[i] * x[i] - 400. * x[i] * x[i + 1] + 2. * x[i] - 2.;
            g[i] = v;
        }
    }
};


#endif	/* ROSENBROK_HPP */

