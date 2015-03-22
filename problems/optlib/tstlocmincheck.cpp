/* 
 * File:   tstlocmincheck.cpp
 * Author: medved
 *
 * Created on June 11, 2014, 1:04 AM
 */

#include <cstdlib>
#include <problems/poly/polyutil.hpp>
#include "locminchecker.hpp"
#include "polyobjective.hpp"

void makeBox(double* x, Box<double> box, double eps) {
    int n  = box.mDim;
    for(int i = 0; i < n; i ++) {
        box.mA[i] = x[i] - eps;
        box.mB[i] = x[i] + eps;
    }
}
/*
 * Test local min checker
 */
int main(int argc, char** argv) {

    std::map<std::string, int> idents;
    idents["x1"] = 0;
    idents["x2"] = 1;
    Polynom<double> poly;
    PolyUtil::fromString("x1^2 + x2^2", idents, poly);
    PolyObjective<double> pobj(&poly);
    double y[2];
    double x1[2] = {0, 0};
    double v  = pobj.func(x1);
    printf("%lf\n", v);
    Box<double> box(2);
    LocalMinChecker<double> lminchk(&pobj);
    int nt = 10000;
    makeBox(x1, box, 0.001);
    bool fl = lminchk.check(x1, y, nt, box);
    BNB_ASSERT(fl);
    x1[0] = 1;
    makeBox(x1, box, 0.001);
    fl = lminchk.check(x1, y, nt, box);
    BNB_ASSERT(!fl);
    return 0;
}

