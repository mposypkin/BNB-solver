/* 
 * File:   test.polyobj1.cpp
 * Author: medved
 *
 * Created on April 4, 2015, 12:36 PM
 */

#include <iostream>
#include <util/poly/polynom.hpp>
#include <util/poly/polynomtxt.hpp>
#include <util/poly/polyutil.hpp>
#include "polyobjective.hpp"


#define POLY "x^2 y^2 + 2 x^2 y + x y + 1"
//#define POLY "x^2 + y^2 + 2 x y"

int main(int argc, char** argv) {

    std::map < std::string, int> idents;
    Polynom<double> poly;
    PolyUtil::fromString(POLY, idents, poly);
    PolynomTxt fmt;
    std::string str = PolyUtil::toString(fmt, poly);
    std::cout << "Polynom: " << str << "\n";
    PolyObjective<double> pobj(&poly);
    int n = pobj.getDim();
    for (int i = 0; i < n; i++) {
        std::string s = PolyUtil::toString(fmt, pobj.getGradient()[i]);
        std::cout << "f_x[" << i << "] = " << s << "\n";
    }
    BNB_ASSERT(pobj.getGradient()[0].getValue(new double[2] {1, 2}) == 18);
    BNB_ASSERT(pobj.getGradient()[1].getValue(new double[2] {1, 2}) == 7);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::string s = PolyUtil::toString(fmt, pobj.getHessian()[i][j]);
            std::cout << "f_xx[" << i << ", " << j << "] = " << s << "\n";
        }
    }
    return 0;
}

