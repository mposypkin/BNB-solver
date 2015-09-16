#ifndef __GRIEN_HPP__
#define __GRIEN_HPP__

#include <math.h>
#include <problems/cover/common/lipzobjective.hpp>

/**
 * Rastringin function
 */
class Grien : public LipzObjective<double> {
public:

    Grien(int n) {
        mDim = n;
    }

    double func(const double* x) {
        double v, u;
        int n;
        u = 0.;
        v = 1.;
        n = getDim();
        for (int i = 0; i < n; i++) {
            u += (x[i] * x[i] / 4000.);
        }
        v += u;
        u = 1.;
        for (int i = 0; i < n; i++) {
            u *= cos(x[i] / sqrt(i + 1.));
        }
        v -= u;
        return v;
    }

    void grad(const double* x, double* g) {
        int n;
        n = getDim();
        for (int i = 0; i < n; i++) {
            double v = x[i] / 2000.;
            double u = 1. / sqrt(i + 1);
            u *= sin(x[i] / sqrt(i + 1));
            for (int j = 0; j < n; j++) {
                if (j != i) {
                    u *= cos(x[j] / sqrt(j + 1));
                }
            }
            v += u;
            g[i] = v;
        }
    }

    void getEigenBounds(const double* a, const double* b, double& emin, double& emax) {
        /*
        emin = -7.755;
        emax = 7.755;
         */
        
        emin = 1./2000. - 2.78;
        emax = 1./2000. + 2.78;
        
        /*
        emin = -0.05;
        emax = 0.05;
         */
    }

#if 0
    double getGradLipzConstant(double* a, double* b) {

        int n;
        double maxv;

        maxv = 0.;
        n = getDim();
        /*
        for(int i = 0; i < n; i ++) {
          double v;
          v = getGradCompLipzConstant(a, b, i);
          maxv = BNBMAX(v, maxv);
        }
        return maxv;
         */
        maxv = 1. / 2000.;
        for (int i = 0; i < n; i++) {
            maxv += 1. / sqrt(i + 1);
        }
        return maxv;
    }

    double getGradCompLipzConstant(double* a, double* b, int i) {
        int n = getDim();
        double v = (1. / 2000.) + (1. / (double) (i + 1));
        for (int j = 0; j < n; j++) {
            if (i != j) {
                v += 1. / sqrt((i + 1) * (j + 1));
            }
        }
        return v;
    }
#endif

private:

    double mA;
};
#endif
