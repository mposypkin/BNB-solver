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
#include <problems/poly/polyutil.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/pareto/multicov/multistate.hpp>
#include <problems/pareto/multicov/multicov.hpp>
#include <util/box/simplboxbag.hpp>
#include <problems/pareto/multiopt/mpsimpbag.hpp>
#include <problems/pareto/multicov/lipbounder.hpp>
#include <problems/pareto/multiopt/mboxlocsearch.hpp>
#include <problems/pareto/multiopt/mgradesc.hpp>
#include <util/ineq/foumotz.hpp>
#include <problems/pareto/multicov/polybounder.hpp>
#include "karp1.hpp"
#include "test1.hpp"
#include "test2.hpp"
#include "evt.hpp"
#include "testosy.hpp"
#include "testbnh.hpp"
#include "testtnk.hpp"

#define EPS 1
#define DELT .1

/*
 * Test Coverage algorithm for multiobjective optimization
 */


int main(int argc, char** argv) {
    std::vector< Polynom<double>* > crit;
    std::vector< Polynom<double>* > cons;
    //MultiData<double> md = Karp1<double>::fillInData();
    //MultiData<double> md = Test1<double>::fillInData();
    //MultiData<double> md = Test2<double>::fillInData();
    //MultiData<double> md = Evt<double>::fillInData();
    MultiData<double> md = TestOSY<double>::fillInData(crit, cons);
    //MultiData<double> md = TestBNH<double>::fillInData(crit, cons);

    PolyBounder<double> pb(crit, cons);

    LipBounder<double> lb(&md);
    LipBounder<double> lbg(&md);
    lbg.getOptions() = LipBounder<double>::Options::LIP_GRAD;
    MultiCov<double> ml(md);
    ml.addBounder(&lb);
    ml.addBounder(&lbg);
    ml.addBounder(&pb);
    ml.setBoundingMethod(3);
    FourierMotzkin<double> fm;
    MultiGradDescent<double> mgd(md, &fm);
    ml.addHeuristic(&mgd);
    ml.setHeuristic(0);

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

