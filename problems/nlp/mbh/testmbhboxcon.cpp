/* 
 * File:   testmbhboxcon.cpp
 * Author: medved
 *
 * Created on September 18, 2015, 8:44 AM
 */

#include "mbhboxcon.hpp"
#include "staticpert.hpp"
#include "util/box/boxutils.hpp"

/**
 * Problem dimension
 */
const int N = 12;

/**
 * Maximal local hops
 */
const int H = 100;

/**
 * Vicinity size
 */
const double  V = 0.01;


/**
 * Objective
 */
class Obj : public Objective <double> {
public:

    /**
     * Constructor
     * @param a right part in the linear constraint
     */
    Obj () {
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
        for (int i = 0; i < n; i ++) {
            v += x[i] * x[i];
        }
        return v;
    }

};


void initbox(Box<double>& box) {
    int n = box.mDim;
    for (int i = 0; i < n; i ++) {
        box.mA[i] = -10;
        box.mB[i] = 10;
    }
}

void initVicinity(Box<double>& box) {
    for (int i = 0; i < N; i ++) {
        box.mA[i] = -V;
        box.mB[i] = V;
    }
}

void initx(double* x) {
    for(int i = 0; i < N; i ++) {
        x[i] = 1;
    }
}

/*
 * Testing the perturbers
 */
int main(int argc, char** argv) {
    Box<double> box(N);
    initbox(box);
    Obj obj;
    NlpProblem<double> prob;
    prob.mBox = box;
    prob.mObj = &obj;
    Box<double> vicinity(N);
    initVicinity(vicinity);
    StaticPerturber<double> perturber(prob, vicinity);
    MBHBoxCon<double> mbhbc(prob, perturber, H);
    double x[N]; 
    initx(x);
        
    std::cout << "Searching in box " << BoxUtils::toString(box) << "\n";
    double v = mbhbc.search(x);
    
    std::cout << "Found v = " << v << "\n";
    VecUtils::vecPrint(N, x);

    return 0;
}

