/* 
 * File:   testmultimc.cpp
 * Author: posypkin
 *
 * Created on April 20, 2011, 3:11 AM
 */

#include <cstdlib>
#include <problems/optlib/polyobjective.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/poly/polynomtxt.hpp>

#include "multimc.hpp"


/*
 * Test for Monte-Carlo algorithm for multiobjective optimization
 */
int main(int argc, char** argv) {

    std::map<std::string, int> idents;
    idents["x1"] = 0;
    idents["x2"] = 1;
    Polynom<double> poly1;
    PolyUtil::fromString( "x1^2 x2^0 + 1", idents, poly1);
    PolyObjective<double> obj1(&poly1);

    
    Polynom<double> poly2;
    PolyUtil::fromString( "x1^0 x2^2 + 1", idents, poly2);
    PolyObjective<double> obj2(&poly2);
    Polynom<double> poly3;
    PolyUtil::fromString("x1^2 + x2^2 - 1", idents, poly3);
    printf("[%s]\n", PolyUtil::toString(PolynomTxt(), poly3).c_str());
    PolyObjective<double> cons(&poly3);


    Box<double> box(2);
    box.mA[0] = -1;
    box.mA[1] = -1;
    box.mB[0] = 1;
    box.mB[1] = 1;
    
    MultiData<double> md(box);
    md.mCriteria.push_back((Objective<double>*)&obj1);
    md.mCriteria.push_back((Objective<double>*)&obj2);
    md.mConstraints.push_back((Objective<double>*)&cons);

    MultiMC<double> mc(md);
    mc.setMaxIters(10000);
    mc.solve();
    printf("result: %s\n", mc.getResult().toString().c_str());

    return 0;
}

