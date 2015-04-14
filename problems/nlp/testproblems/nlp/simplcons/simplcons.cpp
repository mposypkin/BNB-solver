/* 
 * File:   simplcons.cpp
 * Author: medved
 *
 * Created on April 12, 2015, 6:48 PM
 */

#include <iostream>

#include <problems/nlp/common/nlpproblem.hpp>
#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <problems/nlp/cuts/nlprecstore.hpp>
#include <problems/nlp/cuts/dummyrecstore.hpp>
#include <problems/nlp/poly/polysupp.hpp>
#include <problems/nlp/cuts/eigencutfactory.hpp>
#include "problems/nlp/cuts/compcutfactory.hpp"
#include <problems/nlp/cuts/smartcutapplicator.hpp>
#include <util/tree/wfsdfsmanager.hpp>
#include <problems/nlp/bnc/bncsolver.hpp>

#define OBJECTIVE "x^2 + y^2"

#define CONSTRAINT1 "-x^2 - y^2 + 2x"

#define CONSTRAINT2 "-x^2 - y^2 + 2y"

std::map < std::string, int> idents;

PolyObjective<double>* getObjective(const char* str) {
    Polynom<double> *poly = new Polynom<double>();
    PolyUtil::fromString(str, idents, *poly);
    PolynomTxt fmt;
    std::string ss = PolyUtil::toString(fmt, *poly);
    std::cout << "Polynom: " << ss << "\n";
    PolyObjective<double>* pobj = new PolyObjective<double>(poly);
    return pobj;
}

void initbox(Box<double> &box) {
    box.mA[0] = 0.0;
    box.mA[1] = 0.0;
    box.mB[0] = 4.751919;
    box.mB[1] = 5.2520201;
}

BNBNode* makeRootNode() {
    BNBNode* node = new BNBNode;
    BNCSub<double>* sub = new BNCSub<double>();
    Box<double> box(2);
    initbox(box);
    sub->mBox = box;
    node->mData = sub;
    return node;
}
                
/*
 * 
 */
int main(int argc, char** argv) {
    double eps = 0.0001;
    double delta = 0;
    int ldepth = 1;

    /* Initialize box */
    Box<double> box(2);
    initbox(box);

    /* Define objective and constraints*/
    PolyObjective<double>* obj = getObjective(OBJECTIVE);
    PolyObjective<double>* ocons1 = getObjective(CONSTRAINT1);
    Constraint<double> cons1(ocons1, Constraint<double>::Types::INEQUALITY_CONSTRAINT);
    PolyObjective<double>* ocons2 = getObjective(CONSTRAINT2);
    Constraint<double> cons2(ocons2, Constraint<double>::Types::INEQUALITY_CONSTRAINT);

#if 0
    double x[2] = {1, 2};
    std::cout << "v = " << obj->func(x) << "n = " << obj->getDim() << "\n";
    std::cout << "v1 = " << ocons1->func(x) << "n = " << ocons1->getDim() << "\n";
    std::cout << "v2 = " << ocons2->func(x) << "n = " << ocons2->getDim() << "\n" ;
#endif

    /* Setup problem */
    NlpProblem<double> prob;
    prob.mBox = box;
    prob.mCons.push_back(&cons1);
    prob.mCons.push_back(&cons2);
    prob.mObj = obj;
    prob.mVariables.reserve(2);
    prob.mVariables[0] = NlpProblem<double>::VariableTypes::GENERIC;
    prob.mVariables[1] = NlpProblem<double>::VariableTypes::GENERIC;

    /* Setup cut generators */
    /* Cut generator for objective*/
    NlpRecStore<double> ors(10000, &prob);
    PolyEigenSupp objEigenSupp(obj);
    EigenCutFactory<double> objEigenCutFact(&ors, &objEigenSupp, obj, eps);

    /* Cut generators for constraints */
    DummyRecStore<double> drs;
    PolyEigenSupp cons1EigenSupp(ocons1);
    EigenCutFactory<double> cons1EigenCutFact(&drs, &cons1EigenSupp, ocons1, delta);
    PolyEigenSupp cons2EigenSupp(ocons2);
    EigenCutFactory<double> cons2EigenCutFact(&drs, &cons2EigenSupp, ocons2, delta);

    /* Setup composite cut factory  */
    CompCutFactory <double> fact;
    fact.push(&objEigenCutFact);
    fact.push(&cons1EigenCutFact);
    fact.push(&cons2EigenCutFact);

    /* Setup solver */
    SmartCutApplicator<double> sca;
    BNCSolver<double> bnc(&fact, &sca, ldepth);
    BNBTree tree(makeRootNode());
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
    VecUtils::vecPrint(2, (double*)ors.getX());


    return 0;
}

