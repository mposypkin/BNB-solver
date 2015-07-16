#include <stdlib.h>

#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <problems/nlp/cuts/nlprecstore.hpp>
#include <problems/nlp/cuts/unconsrecstore.hpp>
#include <problems/nlp/poly/polysupp.hpp>
#include <problems/nlp/cuts/eigencutfactory.hpp>
#include <problems/nlp/cuts/unconscutfactory.hpp>
#include <problems/nlp/cuts/convcutfactory.hpp>
#include <problems/nlp/cuts/compcutfactory.hpp>
#include <problems/nlp/cuts/smartcutapplicator.hpp>
#include <util/tree/wfsdfsmanager.hpp>
#include <problems/nlp/bnc/bncsolver.hpp>
#include <problems/nlp/bnc/stdboxsplitter.hpp>
#include <problems/optlib/gradboxdesc.hpp>


#include "polybc.hpp"



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
    double eps = .01;
    // Cut analysis depth
    int ldepth = 1;
    // Use or not boxed cut or boxed cut together with normal cut
    int boxedcut = true;


    if (argc != 6)
        BNB_ERROR_REPORT("Usage runpolymi.exe dimension box_size polynom cut_depth use_boxed_cut (0/1/2)");
    n = atoi(argv[1]);
    d = atoi(argv[2]);
    ldepth = atoi(argv[4]);
    boxedcut = atoi(argv[5]);

    PolyBCFactory polybcfact(n, d, argv[3]);
    NlpProblem<double>* nlp = polybcfact.getProb();
    
    std::cout << "Feasible set: " << BoxUtils::toString(nlp->mBox);

#if 0    
    double x[n] = {2, 2};
    double v = nlp->mObj->func(x);
    std::cout << " v = " << v << "\n";
#endif

    /* Setup cut generators */
    /* Cut generator for objective*/
    UnconsRecStore<double> ors(10000, n);
    PolyObjective<double>* obj = dynamic_cast<PolyObjective<double>*> (nlp->mObj);
    PolyEigenSupp objEigenSupp(obj);

    /* Setup eigen based cut factory*/
    EigenCutFactory<double> objEigenCutFact(&ors, &objEigenSupp, obj, eps);

    /* Setup specialized unconstrained cut factory*/
    UnconsCutFactory<double> unconsCutFact(&ors, &objEigenSupp, obj, &(nlp->mBox), eps);
    //UnconsCutFactory<double> unconsCutFact(&ors, &objEigenSupp, obj, NULL, eps);

    /** Setup Convexity cut factory */
    GradLocSearch gls(nlp->mBox, obj);

    ConvCutFactory<double> convCutFact(&ors, &objEigenSupp, obj, &gls);

    /* Setup composite cut factory  */
    CompCutFactory <double> fact;
    fact.push(&objEigenCutFact);
    fact.push(&unconsCutFact);
    //fact.push(&convCutFact);

    /* Setup cut applicator */
    std::cout << "mVariables = " << nlp->mVariables.size() << "\n";
    SmartCutApplicator<double> sca(nlp->mVariables);
    if (boxedcut == 0)
        sca.getOptions() = SmartCutApplicator<double>::Options::CUT_BALL_SIMPLE;
    if (boxedcut == 1)
        sca.getOptions() = SmartCutApplicator<double>::Options::CUT_BALL_BOXED;
    if (boxedcut == 2)
        sca.getOptions() = (SmartCutApplicator<double>::Options::CUT_BALL_SIMPLE
            | SmartCutApplicator<double>::Options::CUT_BALL_BOXED);

    /* Setup splitter */
    StdBoxSplitter<double> splt;

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
    VecUtils::vecPrint(n, (double*) ors.getX());
    
    return 0;
}