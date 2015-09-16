/* 
 * File:   testcov.cpp
 * Author: posypkin
 *
 * Created on April 27, 2011, 7:33 AM
 */

#include <stdio.h>
#include <problems/pareto/multiopt/multidata.hpp>
#include <util/box/box.hpp>
#include <problems/cover/poly/polylipobj.hpp>
#include <util/poly/polyutil.hpp>
#include <util/poly/polynomtxt.hpp>
#include <problems/pareto/multicov/multistate.hpp>
#include <util/box/simplboxbag.hpp>
#include <problems/pareto/multiopt/mpsimpbag.hpp>
#include <problems/pareto/multiopt/mboxlocsearch.hpp>
#include <problems/pareto/multiopt/mgradesc.hpp>
#include <util/ineq/foumotz.hpp>
#include <util/boxbnb/boxsub.hpp>
#include <util/boxbnb/boxsubpool.hpp>


#include "karp1.hpp"
#include "test1.hpp"
#include "test2.hpp"
#include "multicov.hpp"
#include "lipbounder.hpp"

#define EPS 0.1
#define DELT 0.1
/*
 * Test Coverage algorithm for multiobjective optimization
 */

int main(int argc, char** argv) {
//    MultiData<double> md = Karp1<double>::fillInData();
    MultiData<double> md = Test1<double>::fillInData();
    //MultiData<double> md = Test2<double>::fillInData();
    LipBounder<double> lb(&md);
    //lb.getOptions() = LipBounder<double>::Options::LIP_GRAD;
    MultiCov<double> ml(md);
    ml.addBounder(&lb);
    //ml.getOptions() = multicov<double>::Options::USE_LOCAL_SEARCH;
    FourierMotzkin<double> fm;
    MultiGradDescent<double> mgd(md, &fm);
    ml.addHeuristic(&mgd);
    ml.setHeuristic(1);
    MultiState< MPSimpBag < double >, BoxSubPool<double> > ms;
    //ms.mSubBag.setDirection(SubBag<double, WidFirst<double> >::Directions::TAKE_SMALLEST);
    BoxSub<double> sub(md.mBox.mDim, 1);
    sub.mBox = md.mBox;
    ms.mSubBag.put(sub);
    ml.setPrecision(EPS, DELT);
    long long nit = getMaxValue<int>();
    ml.solve(ms, nit);
    printf("%s", ms.mPointBag.toString().c_str());
    printf("\n%lld iterations performed\n", nit);
    for(int i = 0; i < ms.mPointBag.size(); i ++) {
        MultiPoint<double> mp = ms.mPointBag[i];
        if(BNBABS(mp.mCrit.mX[0] + mp.mCrit.mX[1]) > 2 * EPS) {
            printf("i = %d, %lf, %lf, %lf\n", i, mp.mCrit.mX[0], mp.mCrit.mX[1], BNBABS(mp.mCrit.mX[0] - mp.mCrit.mX[1]));
            BNB_ERROR_REPORT("The precision is violated!\n");
        }
    }
}

