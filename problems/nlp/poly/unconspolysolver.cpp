/* 
 * File:   unconspolysolver.cpp
 * Author: medved
 *
 * Created on April 5, 2015, 1:44 PM
 */


#include <math.h>
#include <iostream>
#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polyutil.hpp>

#include <problems/nlp/cuts/eigencutfactory.hpp>
#include <problems/nlp/cuts/lpzcutfactory.hpp>
#include <problems/nlp/cuts/unconscutfactory.hpp>
#include <problems/nlp/cuts/convcutfactory.hpp>
#include <problems/nlp/cuts/compcutfactory.hpp>
#include <problems/nlp/cuts/cututils.hpp>
#include <problems/nlp/cuts/basiccutapplicator.hpp>
#include <problems/nlp/cuts/smartcutapplicator.hpp>
#include <util/tree/wfsdfsmanager.hpp>
#include <util/common/interval.hpp>
#include <problems/nlp/cuts/unconsrecstore.hpp>
#include <problems/optlib/gradboxdesc.hpp>
#include <problems/nlp/bnc/bncsolver.hpp>
#include <problems/nlp/bnc/stdboxsplitter.hpp>

#include "polysupp.hpp"


#if 0
#define POLY "x^2 + y^2 + 2 x y"

void initBox(int n, Box<double>& box) {
    Box<double> nbox(n);
    for (int i = 0; i < n; i++) {
        nbox.mA[i] = -1;
        nbox.mB[i] = 1.5;
    }
    box = nbox;
}
#endif

#if 1
#define POLY "4 x^2 - 2.1 x^4 + 0.33333 x^6 + x y - 4 y^2 + 4 y^4"

void initBox(int n, Box<double>& box) {
    Box<double> nbox(n);
    for (int i = 0; i < n; i++) {
        nbox.mA[i] = -3;
        nbox.mB[i] = 3;
    }
    box = nbox;
}
#endif

class MyStopper : public GradBoxDescent<double>::Stopper {
public:

    bool stopnow(double xdiff, double fdiff, double gnorm, double fval, int n) {
        return fdiff <= 0.0000001;
    }
};

class GradLocSearch : public LocSearch<double> {
public:

    GradLocSearch(Box<double>& box, Objective<double>* obj) {
        mLS = new GradBoxDescent<double>(box, new MyStopper);
        mLS->setObjective(obj);
    }

    double search(double* x) {
        double v;
        mLS->search(x, &v);
        return v;
    }

private:
    GradBoxDescent<double>* mLS;

};

void multinestInitCoeff(SmartArrayPtr<double>& coeff) {
    int n = coeff.size();
    for (int i = 0; i < n; i++)
        coeff[i] = 1;
}

BNBNode* makeRootNode(int n) {
    BNBNode* node = new BNBNode;
    BNCSub<double>* sub = new BNCSub<double>();
    initBox(n, sub->mBox);
    node->mData = sub;
    return node;
}

/*
 * 
 */
int main(int argc, char** argv) {

    int N;
    int ldepth = 1;
    double eps = 0.01;
    if (argc >= 2) {
        eps = atof(argv[1]);
    }
    if (argc >= 3) {
        ldepth = atoi(argv[2]);
    }
    std::map < std::string, int> idents;
    Polynom<double> poly;
    PolyUtil::fromString(POLY, idents, poly);
    N = poly.getNvars();
    PolynomTxt fmt;
    std::string str = PolyUtil::toString(fmt, poly);
    std::cout << "Polynom: " << str << " of degree " << N << "\n";
    PolyObjective<double> pobj(&poly);


    Box<double> box(N);
    initBox(N, box);
    SmartArrayPtr<double> coeff(N);
    // UnconsRecStore<double> rs((new MultinestRefValueProvider(coeff))->getRefValue(N), N);
    UnconsRecStore<double> rs(1000, N);

    PolyEigenSupp esupp(&pobj);

    GradLocSearch gls(box, &pobj);

    //    LpzCutFactory<double> lfact(&rs, &lsupp, &obj, eps);
    EigenCutFactory<double> efact(&rs, &esupp, &pobj, eps);
    UnconsCutFactory<double> ufact(&rs, &esupp, &pobj, eps);
    ConvCutFactory<double> cfact(&rs, &esupp, &pobj, &gls);
    CompCutFactory <double> fact;

    std::cout << "BP 1\n";
#if 0   
    fact.push(&lfact);
#endif

#if 1    
    fact.push(&efact);
#endif    


#if 1    
    fact.push(&ufact);
#endif    

#if 0                   
    fact.push(&cfact);
#endif

#if 0  
    BasicCutApplicator<double> bca;
    BNCSolver<double> bnc(&fact, &bca, ldepth);
#else
    SmartCutApplicator<double> sca;
    StdBoxSplitter<double> splt;
    BNCSolver<double> bnc(&fact, &sca, &splt, ldepth);
#endif



    BNBTree tree(makeRootNode(N));
    BNBNode* root = tree.getRoot();
    WFSDFSManager manager;
    manager.setOptions(WFSDFSManager::Options::DFS);
    manager.reg(root);
    BNCState<double> state(&tree, &manager, &rs);


    bool ru;
    long long int iters = 10000000;
    bnc.solve(iters, state, ru);


    std::cout << iters << " iterations\n";
    std::cout << "Record = " << state.mRecord->getValue() << "\n";
    return 0;
}

