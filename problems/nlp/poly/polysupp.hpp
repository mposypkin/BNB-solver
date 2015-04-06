/* 
 * File:   polysupp.hpp
 * Author: medved
 *
 * Created on April 5, 2015, 1:10 PM
 */

#ifndef POLYSUPP_HPP
#define	POLYSUPP_HPP

#include <problems/nlp/cuts/spectbndssupp.hpp>
#include <problems/nlp/cuts/lpzsupp.hpp>
#include <problems/optlib/polyobjective.hpp>

/**
 * The Supplier of Lipschitz constant for polynomials
 */
class PolyLpzSupp : public LpzSupplier <double> {
public:

    /**
     * Constructor
     * @param obj Polynomial objective
     */
    PolyLpzSupp(PolyObjective<double>* obj) {
        mPolyObj = obj;
    }

    double getLpzConst(const Box<double>& box) {
        double gl = 0, gu = 0;
        int n = mPolyObj->getDim();
        double* a = box.mA;
        double* b = box.mB;
        for (int i = 0; i < n; i++) {
            double l, u;
            mPolyObj->getGradient()[i].getBounds(a, b, &l, &u);
            BNBInterval<double>::sqr(l, u, &l, &u);
            gl += l;
            gu += u;
        }
        return sqrt(gu);
    }

private:
    PolyObjective<double>* mPolyObj;
};



/**
 * The supplier for Eigenvector bounds for polynomials
 * @param obj
 */
class PolyEigenSupp : public SpectrumBoundsSupplier<double> {
public:

    /**
     * Constructor
     * @param obj Polynomial objective
     */
    PolyEigenSupp(PolyObjective<double>* obj) {
        mPolyObj = obj;
    }

    void getBounds(const Box<double>& box, double* lb, double* ub) {
        int n = mPolyObj->getDim();
        double* a = box.mA;
        double* b = box.mB;
        double emin = getMaxValue<double> ();
        double emax = getMinValue<double> ();
        fillHessBounds(a, b);
        for (int i = 0; i < n; i++) {
            double gl, gu, r = 0;
            gl = mHessBounds[i][2 * i];
            gu = mHessBounds[i][2 * i + 1];
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    double l, u;
                    l = mHessBounds[i][2 * j];
                    u = mHessBounds[i][2 * j + 1];
                    double m = BNBMAX(BNBABS(l), BNBABS(u));
                    r += m;
                }
            }
            gl -= r;
            gu += r;
            emin = BNBMIN(emin, gl);
            emax = BNBMAX(emax, gu);
        }
        if (lb != NULL)
            *lb = emin;
        if (ub != NULL)
            *ub = emax;
    }

    
private:

    void fillHessBounds(const double* a, const double* b) {
        double varsm[MAX_POLYNOM_VARS * DEG_INTERVAL_MATRIX_ROW_LEN];
        mPolyObj->getPoly()->fillIntervalMatrix(a, b, varsm);
        int n = mPolyObj->getDim();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                double l, u;
                mPolyObj->getHessian()[i][j].getBoundsCache(varsm, &l, &u);
                mHessBounds[i][2 * j] = l;
                mHessBounds[i][2 * j + 1] = u;
                mHessBounds[j][2 * i] = l;
                mHessBounds[j][2 * i + 1] = u;
            }
        }
    }

    PolyObjective<double>* mPolyObj;
    double mHessBounds[MAX_POLYNOM_VARS][2 * MAX_POLYNOM_VARS];
};

#endif	/* POLYSUPP_HPP */

