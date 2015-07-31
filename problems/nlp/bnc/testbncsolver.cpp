/* 
 * File:   testbncsolver.cpp
 * Author: medved
 *
 * Created on January 10, 2015, 7:37 PM
 */

#include <math.h>

#include <problems/nlp/cuts/lpzcutfactory.hpp>
#include <problems/nlp/cuts/cututils.hpp>
#include <problems/nlp/cuts/basiccutapplicator.hpp>
#include <problems/nlp/cuts/smartcutapplicator.hpp>
#include <util/tree/wfsdfsmanager.hpp>
#include <problems/nlp/cuts/unconsrecstore.hpp>

#include "bncsolver.hpp"
#include "bncstate.hpp"
#include "bncsub.hpp"
#include "stdboxsplitter.hpp"


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
    Box<double> nbox(2);
    nbox.mA[0] = -1;
    nbox.mB[0] = 2;
    nbox.mA[1] = -1;
    nbox.mB[1] = 2;
    box = nbox;
}

class Supp : public LpzSupplier <double> {

    double getLpzConst(const Box<double>& box) {
#if 1
        int n = box.mDim;
        double q = 0;
        for(int i = 0; i < n; i ++) {
            q += BNBMAX(BNBSQR(box.mA[i]), BNBSQR(box.mB[i]));
        }
        return 2 * sqrt(q);
#endif        
#if 0
        return 0.5;
#endif        
    }
};

BNBNode* makeRootNode() {
    BNBNode* node = new BNBNode;
    BNCSub<double>* sub = new BNCSub<double>();
    initBox(sub->mBox);
    node->mData = sub;
    return node;
}

/*
 * 
 */
int main(int argc, char** argv) {

    Box<double> box(2);
    initBox(box);
    Supp supp;
    Obj obj;
    UnconsRecStore<double> rs(100000, 2);
    LpzCutFactory<double> fact(&rs, &supp, &obj, 0.01);

#if 0    
    BasicCutApplicator<double> bca;
    BNCSolver<double> bnc(&fact, &bca, 1);
#else
    SmartCutApplicator<double> sca;
    StdBoxSplitter<double> splt;
    BNCSolver<double> bnc(&fact, &sca, &splt, 1);
#endif

    
    BNBTree tree(makeRootNode());
    BNBNode* root = tree.getRoot();
    WFSDFSManager manager;
    manager.reg(root);
    BNCState<double> state(&manager, &rs);
    state.mForest.push_back(&tree);
    bool ru;
    long long int iters = 10000;
    bnc.solve(iters, state, ru);
    std::cout << iters << " iterations\n";
    std::cout << "Record = " << state.mRecord->getValue() << "\n";
    return 0;
}

