#include <stdlib.h>

#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <problems/nlp/cuts/nlprecstore.hpp>
#include <problems/nlp/cuts/dummyrecstore.hpp>
#include <problems/nlp/poly/polysupp.hpp>
#include <problems/nlp/cuts/eigencutfactory.hpp>
#include <problems/nlp/cuts/pointcutfactory.hpp>
#include <problems/nlp/cuts/compcutfactory.hpp>
#include <problems/nlp/cuts/smartcutapplicator.hpp>
#include <util/tree/wfsdfsmanager.hpp>
#include <problems/nlp/bnc/bncsolver.hpp>
#include <problems/nlp/bnc/stdboxsplitter.hpp>
#include <problems/nlp/bnc/miboxsplitter.hpp>

#include "polymi.hpp"

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
    int d = 3;
    // Accuracy 
    double eps = 0.1;
    // Cut analysis depth
    int ldepth = 1;
    // Use or not boxed cut
    bool boxedcut = true;


    if (argc != 7)
        BNB_ERROR_REPORT("Usage runpolymi.exe dimension box_size polynom cut_depth use_boxed_cut (0/1) record");
    n = atoi(argv[1]);
    d = atoi(argv[2]);
    ldepth = atoi(argv[4]);
    boxedcut = (atoi(argv[5]) == 0) ? false : true;
    double record = atof(argv[6]);

    PolyMIFactory polymifact(n, d, argv[3]);
    NlpProblem<double>* nlp = polymifact.getProb();


    /* Setup cut generators */
    /* Cut generator for objective*/
    NlpRecStore<double> ors(record, nlp);
    PointCutFactory<double> pfact(&ors);
    PolyObjective<double>* obj = dynamic_cast<PolyObjective<double>*> (nlp->mObj);
    PolyEigenSupp objEigenSupp(obj);
    EigenCutFactory<double> objEigenCutFact(&ors, &objEigenSupp, obj, eps);

    /* Setup composite cut factory  */
    CompCutFactory <double> fact;
    fact.push(&pfact);
    fact.push(&objEigenCutFact);

    /* Setup cut applicator */
    SmartCutApplicator<double> sca(nlp->mVariables);
    
    if (boxedcut == 0)
        sca.getOptions() = SmartCutApplicator<double>::Options::CUT_BALL_SIMPLE;
    if (boxedcut == 1)
        sca.getOptions() = SmartCutApplicator<double>::Options::CUT_BALL_BOXED;
    if (boxedcut == 2)
        sca.getOptions() = (SmartCutApplicator<double>::Options::CUT_BALL_SIMPLE
            | SmartCutApplicator<double>::Options::CUT_BALL_BOXED);


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
    BNCState<double> state(&manager, &ors);
    state.mForest.push_back(&tree);


    /* Solving problem */
    bool ru;
    long long int iters = 10000000;

    bnc.solve(iters, state, ru);

    /* Printing results*/
    std::cout << iters << " iterations\n";
    std::cout << "Record = " << state.mRecord->getValue() << "\n";
    VecUtils::vecPrint(n, (double*) ors.getX());

    return 0;
}