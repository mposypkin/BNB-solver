/* 
 * File:   testpert.cpp
 * Author: medved
 *
 * Created on August 5, 2015, 9:55 AM
 */

#include "mbhnlp.hpp"
#include "staticpert.hpp"
#include "util/box/boxutils.hpp"

/**
 * Problem dimension
 */
const int N = 12;

/**
 * Maximal number of items
 * should be less than N / 2
 */
const int K = 3;


/**
 * Constraint's right part
 */
const int A = 100;

/**
 * Maximal local hops
 */
const int H = 10000;


/**
 * Constraint tolerance
 */
const double DELTA = 0;




/**
 * Equality constraint
 */
class ObjCons : public Objective <double> {
public:

    /**
     * Constructor
     * @param a right part in the linear constraint
     */
    ObjCons(double a) : mA(a) {
        /**
         * Setup dimension (number of vars)
         */
        Objective<double>::setDim(N);
    }

    /**
     * Computes constraint value 
     * @param x parametes
     * @return constraint value
     */
    double func(const double* x) {
        double v = 0;
        int n = Objective<double>::mDim;
        for (int i = 0; i < n; i += 2) {
            v += x[i] * x[i + 1];
        }
        double rv = v - mA;
        return rv;
    }

private:
    double mA;

};

/**
 * Objective
 */
class Obj : public Objective <double> {
public:

    Obj() {
        /**
         * Setup dimension (number of vars)
         */
        Objective<double>::setDim(N);
    }

    /**
     * Computes objective value 
     * @param x parametes
     * @return objective value
     */
    double func(const double* x) {
        double v = 0;
        int n = Objective<double>::mDim;
        for (int i = 0; i < n; i += 2) {
            double u = x[i] * x[i + 1];
            v += u * u;
        }
        return v;
    }

};

void initbox(Box<double>& box) {
    int n = box.mDim;
    for (int i = 0; i < n; i += 2) {
        box.mA[i] = 0;
        box.mB[i] = 1;
        box.mA[i + 1] = A / (K + 1) + 1;
        box.mB[i + 1] = 2 * A;
    }
}

void initVicinity(Box<double>& box) {
    for (int i = 0; i < N; i += 2) {
        box.mA[i] = -5;
        box.mB[i] = 5;
        box.mA[i + 1] = -5;
        box.mB[i + 1] = 5;
    }
}

void initx(double* x) {
    for(int i = 0; i < N; i += 2) {
        x[i] = 0;
        x[i + 1] = 40;
    }
}

/*
 * Testing the perturbers
 */
int main(int argc, char** argv) {
    Box<double> box(N);
    initbox(box);
    Obj obj;
    ObjCons objcons(A);
    Constraint<double> cons(&objcons, Constraint<double>::Types::EQUALITY_CONSTRAINT);
    NlpProblem<double> prob;
    prob.mBox = box;
    prob.mCons.push_back(&cons);
    prob.mObj = &obj;
    prob.mVariables.resize(N);
    for (int i = 0; i < N; i += 2) {
        prob.mVariables[i] = NlpProblem<double>::VariableTypes::BOOLEAN;
        prob.mVariables[i + 1] = NlpProblem<double>::VariableTypes::INTEGRAL;
    }

    Box<double> vicinity(N);
    initVicinity(vicinity);
    StaticPerturber<double> perturber(prob, vicinity);
    MBHNlp<double> mbhnlp(prob, perturber, H, DELTA);
    double x[N]; 
    initx(x);
        
    std::cout << "Searching in box " << BoxUtils::toString(box) << "\n";
    mbhnlp.search(x);

    return 0;
}

