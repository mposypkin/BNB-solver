/* 
 * File:   testboxlo.cpp
 * Author: posypkin
 *
 * Created on April 23, 2011, 8:38 AM
 */

#include "util/box/box.hpp"
#include "util/common/utilmacro.hpp"
#include "gradboxdesc.hpp"

class Sqsum : public Objective<double> {
public:

    Sqsum() {
        Objective<double>::setDim(2);
    }

    double func(const double* vec) {
        double x1, x2;
        x1 = vec[0];
        x2 = vec[1];
        return x1 * x1 + x2*x2;
    }

    void grad(const double* vec, double* g) {
        double x1, x2;
        x1 = vec[0];
        x2 = vec[1];
        g[0] = 2 * x1;
        g[1] = 2 * x2;
    }

};

int testMethod(LocalBoxOptimizer<double>& opt) {
    Sqsum obj;
    opt.setObjective(&obj);
    Box<double> box(2);
    opt.setBox(box);
    box.mA[0] = -1;
    box.mA[1] = -3;
    box.mB[0] = 1;
    box.mB[1] = -1;
    double x[2] = {.25, -1.25};
    double v;
    double eps = 0.001;
    opt.search(x, &v);
    if (BNBMAX(BNBABS(x[0] - 0), BNBABS(x[1] - (-1))) < eps)
        return 0;
    else
        return 1;
}

class MyStopper : public GradBoxDescent<double>::Stopper {
public:
    /**
     * Returns true when the search should stop
     * @param xdiff difference between old and new x
     * @param fdiff difference between old and new f value
     * @param gnorm gradient norm
     * @param fval function value
     * @param n current step number 
     */
    bool stopnow(double xdiff, double fdiff, double gnorm, double fval, int n) {
        if(fdiff < 0.001) 
            return true;
        else 
            return false;
    }

};

/*
 * Testing local optimization methods with box constraints
 */
int main(int argc, char** argv) {
    Box<double> box(2);
    MyStopper mystop;
    GradBoxDescent<double> gbd(box, &mystop);
    int rc;
    rc = testMethod(gbd);
    return rc;
}

