/* 
 * File:   runmisqsum.cpp
 * Author: medved
 *
 * Created on May 5, 2015, 1:38 PM
 */

#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <problems/nlp/cuts/nlprecstore.hpp>
#include <problems/nlp/cuts/dummyrecstore.hpp>
#include <problems/nlp/poly/polysupp.hpp>
#include <problems/nlp/cuts/eigencutfactory.hpp>
#include <problems/nlp/cuts/compcutfactory.hpp>
#include <problems/nlp/cuts/smartcutapplicator.hpp>
#include <util/tree/wfsdfsmanager.hpp>
#include <problems/nlp/bnc/bncsolver.hpp>
#include <problems/nlp/bnc/stdboxsplitter.hpp>
#include <problems/nlp/bnc/miboxsplitter.hpp>

#include "misqsumfact.hpp"

static BNBNode* makeRootNode(NlpProblem<double>& nlp) {
    BNBNode* node = new BNBNode;
    BNCSub<double>* sub = new BNCSub<double>();
    sub->mBox = nlp.mBox;
    node->mData = sub;
    return node;
}

/*
 * 
 */
int main(int argc, char** argv) {
    // Dimension
    int n = 2;
    // Box size
    int d = 10;
    // Accuracy 
    double eps = 0.1;
    // Cut analysis depth
    int ldepth = 1;

    MISqSumFactory sqfact(n, d);
    NlpProblem<double>* nlp = sqfact.getProb();

#if 0    
    double x[n] = {2, 2};
    double v = nlp->mObj->func(x);
    std::cout << " v = " << v << "\n";
#endif

    /* Setup cut generators */
    /* Cut generator for objective*/
    NlpRecStore<double> ors(10000, nlp);
    PolyObjective<double>* obj = dynamic_cast<PolyObjective<double>*> (nlp->mObj);
    PolyEigenSupp objEigenSupp(obj);
    EigenCutFactory<double> objEigenCutFact(&ors, &objEigenSupp, obj, eps);

    /* Setup composite cut factory  */
    CompCutFactory <double> fact;
    fact.push(&objEigenCutFact);

    /* Setup cut applicator */
    std::cout << "mVariables = " << nlp->mVariables.size() << "\n";
    SmartCutApplicator<double> sca(nlp->mVariables);
    //sca.getOptions() |= SmartCutApplicator<double>::Options::CUT_BALL_BOXED;

    /* Setup splitter */
    //StdBoxSplitter<double> splt;
    MIBoxSplitter<double> splt(nlp->mVariables);
    
    /* Setup solver */
    BNCSolver<double> bnc(&fact, &sca, &splt, ldepth);
    BNBTree tree(makeRootNode(*nlp));
    BNBNode* root = tree.getRoot();
    WFSDFSManager manager;
    manager.setOptions(WFSDFSManager::Options::DFS);
    manager.reg(root);
    BNCState<double> state(&tree, &manager, &ors);


    /* Solving problem */
    bool ru;
    long long int iters = 10000000;

    std::cout << "Record = " << state.mRecord->getValue() << "\n";

    bnc.solve(iters, state, ru);

    /* Printing results*/
    std::cout << iters << " iterations\n";
    std::cout << "Record = " << state.mRecord->getValue() << "\n";
    VecUtils::vecPrint(2, (double*) ors.getX());



    return 0;
}

