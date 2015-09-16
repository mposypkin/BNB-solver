/*
 * File:   testmultimc.cpp
 * Author: posypkin
 *
 * Created on April 20, 2011, 3:11 AM
 */

#include <stdio.h>
#include <string.h>
#include <problems/pareto/multiopt/multimc.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <util/poly/polyutil.hpp>
#include <util/poly/polynomtxt.hpp>
#include <problems/optlib/gradboxdesc.hpp>
#include <problems/pareto/multiopt/mboxlocsearch.hpp>
#include <problems/pareto/multiopt/mgradesc.hpp>
#include <util/ineq/foumotz.hpp>

#include "karp1.hpp"
#include "test1.hpp"
#include "evt.hpp"

#define NITER 500
#define MAXBAGSIZE 500000

void message(const char* s) {
    printf("use: %s nols | ls1 | ls2\n", s);
    exit(1);
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
            if(xdiff <= 0.001) {
                return true;
            }
        }
    };
 
/*
 * Test for Monte-Carlo algorithm for multiobjective optimization
 */
int main(int argc, char** argv) {

    if (argc != 2)
        message(argv[0]);

    //MultiData<double> md = Karp1<double>::fillInData();
    MultiData<double> md = Test1<double>::fillInData();
    //MultiData<double> md = Evt<double>::fillInData();
    MultiMC<double> mc(md);
    mc.setMaxIters(NITER);
    mc.setMaxBagSize(MAXBAGSIZE);

    if (!strcmp(argv[1], "nols")) {
        mc.solve();
    } else if (!strcmp(argv[1], "ls1")) {
        mc.getOptions() |= MultiMC<double>::Options::USE_LOCAL_SEARCH;
        MyStopper stopper;
        GradBoxDescent<double> gbd(md.mBox, &stopper);
        std::vector<double> coe;
        coe.push_back(1.);
        coe.push_back(2.);
        MBoxLocalSearch<double> mbls(md, coe, &gbd);
        mbls.getOptions() |= MBoxLocalSearch<double>::Options::SHAKE_WEIGHTS;
        mc.setLocalSearch(&mbls);
        mc.solve();
    } else if (!strcmp(argv[1], "ls2")) {
        mc.getOptions() |= MultiMC<double>::Options::USE_LOCAL_SEARCH;
        FourierMotzkin<double> fm;
        MultiGradDescent<double> mgd(md, &fm);
        mc.setLocalSearch(&mgd);
        mc.solve();
    } else {
        message(argv[0]);
    }
    printf("%s:\n %d iters \n %d bag size \n bag: \n%s\n",
            argv[1], mc.getCounters().mIters, mc.getCounters().mBagSize, mc.getResult().toString().c_str());

}


