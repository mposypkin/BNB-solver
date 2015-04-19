/* 
 * File:   dejongsupp.hpp
 * Author: medved
 *
 * Created on April 18, 2015, 8:36 PM
 */

#ifndef DEJONGSUPP_HPP
#define	DEJONGSUPP_HPP

/**
 * Eigenvalues bounds supplier 
 * 
 */
class DejongEigenSupp : public SpectrumBoundsSupplier<double> {
public:

    void getBounds(const Box<double>& box, double* lb, double* ub) {
        if (lb)
            *lb = 2;
        if (ub)
            *ub = 2;
    }

};

/**
 * Lipschitz constant upper bound supplier
 */
class DejongLpzSupp : public LpzSupplier<double> {
public:

    double getLpzConst(const Box<double>& box) {
        int n;
        double norm = 0;
        double *a = box.mA;
        double *b = box.mB;
        n = box.mDim;
        for (int i = 0; i < n; i++) {
            norm += 4 * BNBMAX(a[i] * a[i], b[i] * b[i]);
        }
        return sqrt(norm);
        //return 1;
    }

};


#endif	/* DEJONGSUPP_HPP */

