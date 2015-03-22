/* 
 * File:   testcov.cpp
 * Author: posypkin
 *
 * Created on April 27, 2011, 7:33 AM
 */

#include <stdio.h>
#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/pareto/multicov/multistate.hpp>
#include <problems/pareto/multicov/multicov.hpp>
#include <util/box/simplboxbag.hpp>
#include <problems/pareto/multiopt/mpsimpbag.hpp>
#include <problems/pareto/multicov/lipbounder.hpp>
#include <problems/pareto/multiopt/mboxlocsearch.hpp>
#include <problems/pareto/multiopt/mgradesc.hpp>
#include "testtnk.hpp"
#include "tnkbounder.hpp"

#define EPS .01
#define DELT .01

/*
 * Test Coverage algorithm for multiobjective optimization
 */


int main(int argc, char** argv) {
    MultiData<double> md = TestTNK::fillInData();
    MultiCov<double> ml(md);
    TNKBounder tb;
    ml.addBounder(&tb);
    ml.setBoundingMethod(1);

    MultiState< MPSimpBag < double >, BoxSubPool<double> > ms;
    BoxSub<double> sub(md.mBox.mDim, 1);
    sub.mBox = md.mBox;
    ms.mSubBag.put(sub);
    ml.setPrecision(EPS, DELT);
    long long int nit = getMaxValue<int>();
    ml.solve(ms, nit);
    printf("%s", ms.mPointBag.toString().c_str());
    printf("\n%lld iterations performed\n", nit);
}

