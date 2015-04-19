/* 
 * File:   dejong.hpp
 * Author: medved
 *
 * Created on April 18, 2015, 8:35 PM
 */

#ifndef DEJONG_HPP
#define	DEJONG_HPP

#include <util/common/vec.hpp>
#include <util/common/interval.hpp>

/**
 * Dejong function
 */
class Dejong : public Objective<double> {
public:

    /**
     * Constructor
     * @param n dimension
     */
    Dejong(int n) {
        mDim = n;
    }

    double func(const double* x) {
        double v;
        int n;
        v = 0.;
        n = getDim();
        for (int i = 0; i < (n - 1); i++) {
            v += x[i] * x[i];
        }
        return v;
    }

    void grad(const double* x, double* g) {
        int n;
        n = getDim();
        for (int i = 0; i < n; i++) {
            g[i] = 2 * x[i];
        }
    }
};



#endif	/* DEJONG_HPP */

