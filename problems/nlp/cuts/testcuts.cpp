/* 
 * File:   testcuts.cpp
 * Author: medved
 *
 * Created on November 2, 2014, 6:03 PM
 */

#include <cstdlib>
#include <math.h>
#include <iostream>
#include <vector>
#include "lpzcutfactory.hpp"
#include "cututils.hpp"
#include "basiccutapplicator.hpp"
#include "smartcutapplicator.hpp"
#include "unconsrecstore.hpp"

/*
 * 
 */
class Obj : public Objective <double> {
public:

    Obj() {
        /**
         * Setup dimension (number of vars)
         */
        Objective<double>::setDim(2);
    }

    /**
     * Computes objective function value f(x) = x0^2 + x1^2
     * @param x parametes
     * @return constraint value
     */
    double func(const double* x) {
        return x[0] * x[0] + x[1] * x[1];
    }

};

void initBox(Box<double>& box) {
    box.mA[0] = 0;
    box.mB[0] = 4;
    box.mA[1] = 0;
    box.mB[1] = 4;
}

class Supp : public LpzSupplier <double>{

    double getLpzConst(const Box<double>& box) {
        return 10;
    }
};

int main(int argc, char** argv) {
    Box<double> box(2);
    initBox(box);
    Supp supp;
    Obj obj;
    UnconsRecStore<double> rs(0, 2);
    LpzCutFactory<double> fact(&rs, &supp, &obj, 0);
    std::vector< Cut<double> > cutv;
    fact.getCuts(box, cutv);
    for (auto o : cutv) {
        std::cout << CutUtils<double> ::toString(o) << "\n";
    }
    Cut<double> cut = cutv.at(0);
    cut.mR = sqrt(5);
    std::vector< Cut<double> > cuts;
    cuts.push_back(cut);
    std::vector< Box<double> > bv;
    SmartCutApplicator<double> sca;
    sca.ApplyCut(cuts, box, bv);
    std::cout << ((bv.size() == 2) ? "cut successful\n" : "nothing cutted\n");
    BNB_ASSERT(bv.size() == 2);
    std::vector< Box<double> > bv1;
    std::cout << bv.size() << " box in the list\n";
    for (auto o : bv) {
        std::cout << BoxUtils::toString(o) << "\n";
    }
#if 0
    for (auto o : bv) {
        CutUtils<double>::ApplyCut(cut, o, bv1);
    }
    std::cout << bv1.size() << " box in the list\n";
    for (auto o : bv1) {
        std::cout << BoxUtils::toString(o) << "\n";
    }
    BNB_ASSERT(bv1.size() == 4);
#endif
    
    BasicCutApplicator<double> bca;
    std::vector< Box<double> > bv2;
    bca.ApplyCut(cuts, box, bv2);
    std::cout << "The 1st basic cut " << ((bv2.size() == 2) ? "succeded" : "not succeeded") << "\n";
    BNB_ASSERT(bv2.size() == 1);
    cut.mR = 3;
    cuts.clear();
    cuts.push_back(cut);
    bv2.clear();
    bca.ApplyCut(cuts, box, bv2);
    std::cout << "The 2nd basic cut " << ((bv2.size() == 0) ? "succeded" : "not succeeded")  << "\n";
    BNB_ASSERT(bv2.empty());
    
    return 0;
}

