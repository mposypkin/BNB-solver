/* 
 * File:   test.polysupp.cpp
 * Author: medved
 * Testing of polynomial suppliers
 * Created on April 6, 2015, 12:48 PM
 */
#include <iostream>
#include <util/poly/polynom.hpp>
#include <util/poly/polynomtxt.hpp>
#include <util/poly/polyutil.hpp>

#include "polysupp.hpp"

#define POLY "x^2 + y^2 + 2 x y"

/*
 * 
 */



int main(int argc, char** argv) {
    
    std::map < std::string, int> idents;
    Polynom<double> poly;
    PolyUtil::fromString(POLY, idents, poly);
    PolynomTxt fmt;
    std::string str = PolyUtil::toString(fmt, poly);
    std::cout << "Polynom: " << str << "\n";
    PolyObjective<double> pobj(&poly);
    PolyLpzSupp lsupp(&pobj);
    Box<double> box(2);
    box.mA[0] = -1;
    box.mA[1] = -1;
    box.mB[0] = 1;
    box.mB[1] = 1;
    std::cout << "Lipschitz constant " << lsupp.getLpzConst(box) << "\n";
    BNB_ASSERT((BNBSQR(lsupp.getLpzConst(box)) - 32) < 0.0001);
    PolyEigenSupp esupp(&pobj);    
    double a, b;
    esupp.getBounds(box, &a, &b);
    std::cout << "a = " << a << ", b = " << b << "\n";
    BNB_ASSERT((a == 0) && (b == 4));
}

