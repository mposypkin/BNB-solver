/* 
 * File:   testeigencuts.cpp
 * Author: medved
 *
 * Created on February 1, 2015, 2:13 PM
 */

#include <iostream>

#include "cututils.hpp"
#include "eigencutfactory.hpp"
#include "unconsrecstore.hpp"

void initBox(Box<double>& box) {
    box.mA[0] = 0;
    box.mB[0] = 2;
    box.mA[1] = 0;
    box.mB[1] = 2;
}

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
     * Computes objective function value f(x) = 1/2 (x0^2 + x1^2)
     * @param x parametes
     * @return constraint value
     */
    double func(const double* x) {
        return 0.5 * (x[0] * x[0] + x[1] * x[1]);
    }

    void grad(const double* x, double* g) {
        g[0] = x[0];
        g[1] = x[1];
    }
};

class ObjOne : public Objective <double> {
public:

    ObjOne() {
        /**
         * Setup dimension (number of vars)
         */
        Objective<double>::setDim(2);
    }

    /**
     * Computes objective function value f(x) = 1/2 (x0^2 - x1^2)
     * @param x parametes
     * @return constraint value
     */
    double func(const double* x) {
        return 0.5 * (x[0] * x[0] - x[1] * x[1]);
    }

    void grad(const double* x, double* g) {
        g[0] = x[0];
        g[1] = -x[1];
    }
};

class ObjTwo : public Objective <double> {
public:

    ObjTwo() {
        /**
         * Setup dimension (number of vars)
         */
        Objective<double>::setDim(2);
    }

    /**
     * Computes objective function value f(x) = 1/2 x0^2 + x1
     * @param x parametes
     * @return constraint value
     */
    double func(const double* x) {
        return 0.5 * x[0] * x[0] + x[1];
    }

    void grad(const double* x, double* g) {
        g[0] = x[0];
        g[1] = 1;
    }
};

class Supp : public SpectrumBoundsSupplier<double> {
public:

    Supp(double lb) {
        mLB = lb;
    }

    void getBounds(const Box<double>& box, double* lb, double* ub) {
        if(lb != NULL)
            *lb = mLB;
    }

    void setLB(double lb) {
        mLB = lb;
    }

private:
    double mLB;

};

/*
 * 
 */
int main(int argc, char** argv) {
    Box<double> box(2);
    initBox(box);
    Supp supp(1);
    Obj obj;
    UnconsRecStore<double> rs(3, 2);
    EigenCutFactory<double> fact(&rs, &supp, &obj, 0.68);
    std::vector< Cut<double> > cuts;
    fact.getCuts(box, cuts);

    supp.setLB(-1);
    ObjOne objone;
    EigenCutFactory<double> factone(&rs, &supp, &objone, 0.28);
    factone.getCuts(box, cuts);

    supp.setLB(0);
    ObjTwo objtwo;
    EigenCutFactory<double> facttwo(&rs, &supp, &objtwo, 0.5);
    facttwo.getCuts(box, cuts);
    for (auto o : cuts) {
        std::cout << CutUtils<double> ::toString(o) << "\n";
    }

    BNB_ASSERT((cuts[0].mType == Cut<double>::CutType::OUTER_BALL) && (BNBABS(cuts[0].mR - 0.8) < 0.001) && (cuts[0].mC[0] == 0) && (cuts[0].mC[1] == 0));
    BNB_ASSERT((cuts[1].mType == Cut<double>::CutType::INNER_BALL) && (BNBABS(cuts[1].mR - 1.6) < 0.001) && (cuts[1].mC[0] == 2) && (cuts[1].mC[1] == 0));
    BNB_ASSERT((cuts[2].mType == Cut<double>::CutType::LINEAR) &&(cuts[2].mR == 2) && (cuts[2].mC[0] == 1) && (cuts[2].mC[1] == 1));
    return 0;
}

