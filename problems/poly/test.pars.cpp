/* 
 * File:   testpars.cpp
 * Author: posypkin
 *
 * Created on April 21, 2011, 8:29 AM
 */

#include <cstdlib>
#include "polypars.hpp"
#include "polynomtxt.hpp"
#include "util/common/bnberrcheck.hpp"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    std::map<std::string, int> idents;
    int p = 0;
    char* s = "2 x1^2 x2^3";
    Monom<double> mon = PolyPars::parseMonom<double>(s, idents, -1, &p);
    double vars1[3] = {1., 2.};    
    BNB_ASSERT(BNBABS(mon.getValue(vars1) + 16.) < 0.00001);

    s = "2 x1^2 x2^3 -5x1^4 + 6 x3^2 + x2";
    
    Polynom<double> poly;    
    PolyPars::parsePolynom(s, idents, poly);
    double vars2[3] = {1., 0., 2.};
    double v = poly.getValue(vars2);
    BNB_ASSERT(BNBABS(v - 19.) < 0.00001);
}

