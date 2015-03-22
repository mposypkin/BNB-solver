/* 
 * File:   multinestrefv.hpp
 * Author: medved
 *
 * Created on February 11, 2015, 10:17 PM
 */

#ifndef MULTINESTREFV_HPP
#define	MULTINESTREFV_HPP

/**
 * Reference value supplier
 */
class MultinestRefValueProvider {
public:

    /**
     * Constructor
     * @param coeff coefficients of the Multinest function
     */
    MultinestRefValueProvider(SmartArrayPtr<double>& coeff) {
        mCoeff = coeff;
    }

    double getRefValue(int n) {
        double v = 0;
        for (int i = 0; i < n; i++) {
            v += mCoeff[i] * 2.25;
        }
        return -v;
    }

private:
    SmartArrayPtr<double> mCoeff;
};


#endif	/* MULTINESTREFV_HPP */

