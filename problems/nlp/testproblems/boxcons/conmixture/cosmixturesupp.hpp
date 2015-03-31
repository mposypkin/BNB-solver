/* 
 * File:   cosmixturesupp.hpp
 * Author: medved
 *
 * Created on March 23, 2015, 10:54 PM
 */

#ifndef COSMIXTURESUPP_HPP
#define	COSMIXTURESUPP_HPP

#include <problems/nlp/cuts/spectbndssupp.hpp>
#include <util/common/interval.hpp>

/**
 * The supplier of bounds for eigenvalues
 */
class CosMixtureEigenSupp : public SpectrumBoundsSupplier<double> {
public:

    /**
     * Constructor 
     * @param a 1st coefficient
     * @param b 2nd coefficient
     */
    CosMixtureEigenSupp(double a = 0.1, double b = 5 * M_PI) : mA(a), mB(b) {        
    }

    void getBounds(const Box<double>& box, double* lb, double* ub) {
        double leb, ueb;
        int n = box.mDim;
        for (int i = 0; i < n; i++) {
            double a, b;
            BNBInterval<double>::cos(mB * box.mA[i], mB * box.mB[i], &a, &b);
            if (i == 0) {
                leb = 2 + mA * mB * mB * a;
                ueb = 2 + mA * mB * mB * b;
            } else {
                leb = BNBMIN(leb, 2 + mA * mB * mB * a);
                ueb = BNBMAX(ueb, 2 + mA * mB * mB * b);
            }
        }
        if (lb != NULL)
            *lb = leb;
        if (ub != NULL)
            *ub = ueb;
    }

private:
    double mA, mB;
};

#endif	/* COSMIXTURESUPP_HPP */

