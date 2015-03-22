/* 
 * File:   rosenbroksupp.hpp
 * Author: medved
 *
 * Created on February 11, 2015, 10:23 PM
 */

#ifndef ROSENBROKSUPP_HPP
#define	ROSENBROKSUPP_HPP

#include <problems/nlp/cuts/spectbndssupp.hpp>
#include <problems/nlp/cuts/lpzsupp.hpp>
#include <util/common/interval.hpp>

/**
 * Eigenvalues bounds supplier 
 * 
 */
class RosenbrokEigenSupp : public SpectrumBoundsSupplier<double> {
public:

    void getBounds(const Box<double>& box, double* lb, double* ub) {
        int n;
        n = box.mDim;
        double* a = box.mA;
        double* b = box.mB;
        double emax = getMinValue<double>();
        double emin = getMaxValue<double>();
        for (int i = 0; i < n; i++) {
            double lemax;
            double lemin;
            double dlo = 0;
            double dup = 0;
            if (i != (n - 1)) {
                BNBInterval<double>::sqr(a[i], b[i], &dlo, &dup);
                BNBInterval<double>::diff(1200 * dlo, 1200 * dup, 400 * a[i + 1], 400 * a[i + 1], &dlo, &dup);
                dlo += 2;
                dup += 2;
            }
            if (i != 0) {
                dlo += 200;
                dup += 200;
            }
            lemin = dlo;
            lemax = dup;
            if (i != (n - 1)) {
                double xabs = BNBMAX(BNBABS(a[i]), BNBABS(b[i]));
                lemin -= 400 * xabs;
                lemax += 400 * xabs;
            }
            if (i != 0) {
                double xabs = BNBMAX(BNBABS(a[i - 1]), BNBABS(b[i - 1]));
                lemax += 400 * xabs;
                lemin -= 400 * xabs;
            }
            emin = BNBMIN(emin, lemin);
            emax = BNBMAX(emax, lemax);
        }
        if (lb)
            *lb = emin;
        if (ub)
            *ub = emax;
    }

};

/**
 * Lipschitz constant upper bound supplier
 */
class RosenbrokLpzSupp : public LpzSupplier<double> {
public:

    double getLpzConst(const Box<double>& box) {
        int n;
        double norm;
        double *a = box.mA;
        double *b = box.mB;
        norm = 0.;
        n = box.mDim;
        for (int i = 0; i < n; i++) {
            double v;
            double xm;

            v = 0.;
            xm = BNBMAX(BNBABS(a[i]), BNBABS(b[i]));
            if (i != 0) {
                double xmp;
                xmp = BNBMAX(BNBABS(a[i - 1]), BNBABS(b[i - 1]));
                v += 200. * (xm + xmp * xmp);
            }
            if (i != (n - 1)) {
                double xmn;
                xmn = BNBMAX(BNBABS(a[i + 1]), BNBABS(b[i + 1]));
                v += 400. * xm * xm * xm + 400. * xm * xmn + 2. * xm + 2.;
            }
            norm += BNBSQR(v);
        }
        return sqrt(norm);
        //return 1;
    }

};


#endif	/* ROSENBROKSUPP_HPP */

