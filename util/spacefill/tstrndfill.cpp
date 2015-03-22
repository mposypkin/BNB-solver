/* 
 * File:   tstrndfill.cpp
 * Author: medved
 *
 * Created on June 10, 2014, 11:29 PM
 */

#include <cstdlib>
#include "util/common/vec.hpp"
#include "util/common/vec.hpp"
#include "util/common/bnberrcheck.hpp"
#include "rndfill.hpp"

const int N = 5;
const int n = 3;

/*
 * Test RndFill utility
 */
int main(int argc, char** argv) {
    bool interactive = (argc > 1);
    Box<double> box(n);
    for (int i = 0; i < n; i++) {
        box.mA[i] = -1;
        box.mB[i] = 2;
    }

    RndFill<double> rf(box);
    for (int i = 0; i < N; i++) {
        double x[n];
        rf.getPoint(x);
        if (interactive)
            VecUtils::vecPrint(n, x);
        else {
            int p;
            double min = VecUtils::min(n, (double*) box.mA, &p);
            BNB_ASSERT(min >= -1);
            double max = VecUtils::max(n,  (double*) box.mB, &p);
            BNB_ASSERT(max <= 2);
        }
    }
    return 0;
}

