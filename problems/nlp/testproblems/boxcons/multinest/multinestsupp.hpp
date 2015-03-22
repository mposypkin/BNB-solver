/* 
 * File:   multinestsupp.hpp
 * Author: medved
 *
 * Created on February 11, 2015, 10:12 PM
 */

#ifndef MULTINESTSUPP_HPP
#define	MULTINESTSUPP_HPP

#include <problems/nlp/cuts/spectbndssupp.hpp>
#include <problems/nlp/cuts/lpzsupp.hpp>

/**
 * Eigenvalues bounds supplier 
 * 
 * @param coeff
 */
class MultinestEigenSupp : public SpectrumBoundsSupplier<double> {
public:

    MultinestEigenSupp(SmartArrayPtr<double>& coeff) {
        mCoeff = coeff;
    }

    void getBounds(const Box<double>& box, double* lb, double* ub) {
        double leb, ueb;
        int n = box.mDim;
        for (int i = 0; i < n; i++) {
            double a, b;
            BNBInterval<double>::sqr(box.mA[i], box.mB[i], &a, &b);
            if (i == 0) {
                leb = mCoeff[i] * (12 * a - 10);
                ueb = mCoeff[i] * (12 * b - 10);
            } else {
                leb = BNBMIN(leb, mCoeff[i] * (12 * a - 10));
                ueb = BNBMAX(ueb, mCoeff[i] * (12 * b - 10));
            }
        }
        if (lb != NULL)
            *lb = leb;
        if (ub != NULL)
            *ub = ueb;
    }

private:
    SmartArrayPtr<double> mCoeff;
};

class MultinestLpzSupp : public LpzSupplier <double> {
public:
    
    MultinestLpzSupp(SmartArrayPtr<double>& coeff) {
        mCoeff = coeff;
    }

    double getLpzConst(const Box<double>& box) {
        double v = 0;

        int n = box.mDim;
        for (int i = 0; i < n; i++) {
            double a, b, c, d, e, f;

            BNBInterval<double>::degree(3, box.mA[0], box.mB[0], &a, &b);
            BNBInterval<double>::diff(4 * a, 4 * b, 10 * box.mA[0], 10 * box.mB[0], &c, &d);
            BNBInterval<double>::sqr(c, d, &e, &f);
            v += mCoeff[i] * f;
        }
        return sqrt(v);
    }

private:
    SmartArrayPtr<double> mCoeff;
};


#endif	/* MULTINESTSUPP_HPP */

