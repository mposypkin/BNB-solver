/* 
 * File:   testbncsolver.cpp
 * Author: medved
 *
 * Created on January 10, 2015, 7:37 PM
 */

#include <math.h>

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
#include <problems/nlp/testproblems/boxcons/multinest/multinest.hpp>
#include <problems/nlp/testproblems/boxcons/multinest/multinestsupp.hpp>
#include <problems/nlp/testproblems/boxcons/multinest/multinestrefv.hpp>
#include <problems/nlp/testproblems/boxcons/rosenbrok/rosenbrok.hpp>
#include <problems/nlp/testproblems/boxcons/rosenbrok/rosenbroksupp.hpp>
#include <problems/nlp/testproblems/boxcons/rosenbrok/rosenbrokrefv.hpp>
#include <problems/nlp/testproblems/boxcons/conmixture/cosmixture.hpp>
#include <problems/nlp/testproblems/boxcons/conmixture/cosmixturesupp.hpp>
#include <problems/nlp/testproblems/boxcons/dejong/dejong.hpp>
#include <problems/nlp/testproblems/boxcons/dejong/dejongsupp.hpp>

#include "bncsolver.hpp"
#include "bncstate.hpp"
#include "bncsub.hpp"
#include "stdboxsplitter.hpp"

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

void initBox(int n, Box<double>& box) {
    Box<double> nbox(n);
    for (int i = 0; i < n; i++) {
        nbox.mA[i] = -1;
        nbox.mB[i] = 1;
    }
    box = nbox;
}

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

    int N = 2;
    int ldepth = 1;
    double eps = 0.01;
    if (argc >= 2) {
        N = atoi(argv[1]);
    }
    if (argc >= 3) {
        eps = atof(argv[2]);
    }
    if (argc >= 4) {
        ldepth = atoi(argv[3]);
    }

    Box<double> box(N);
    initBox(N, box);
    SmartArrayPtr<double> coeff(N);
    // UnconsRecStore<double> rs((new MultinestRefValueProvider(coeff))->getRefValue(N), N);
    UnconsRecStore<double> rs(1000, N);

#if 1
    Dejong obj(N);
    DejongLpzSupp lsupp;
    DejongEigenSupp esupp;
#endif

#if 0
    Rosenbrok obj(N);
    RosenbrokLpzSupp lsupp;
    RosenbrokEigenSupp esupp;
#endif

#if 0
    multinestInitCoeff(coeff);
    MultiNestObj obj(N, coeff);
    MultinestLpzSupp lsupp(coeff);
    MultinestEigenSupp esupp(coeff);
#endif

#if 0
    CosMixture obj(N);
    //RosenbrokLpzSupp lsupp;
    CosMixtureEigenSupp esupp;
#endif


    GradLocSearch gls(box, &obj);

    LpzCutFactory<double> lfact(&rs, &lsupp, &obj, eps);
    EigenCutFactory<double> efact(&rs, &esupp, &obj, eps);
    UnconsCutFactory<double> ufact(&rs, &esupp, &obj, &box, eps);
    ConvCutFactory<double> cfact(&rs, &esupp, &obj, &gls);
    CompCutFactory <double> fact;

#if 1
    fact.push(&lfact);
#endif

#if 0
    fact.push(&efact);
#endif    


#if 0    
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
    BNCState<double> state(&manager, &rs);
    state.mForest.push_back(&tree);


    bool ru;
    long long int iters = 10000000;
    bnc.solve(iters, state, ru);


    std::cout << iters << " iterations\n";
    std::cout << "Record = " << state.mRecord->getValue() << "\n";
    //std::cout << "Reference minimum value is " << (new MultinestRefValueProvider(coeff))->getRefValue(N) << "\n";
    return 0;
}

