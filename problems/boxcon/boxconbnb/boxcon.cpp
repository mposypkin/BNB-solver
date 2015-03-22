/* 
 * File:   boxcon.cpp
 * Author: mikdebian
 *
 * Created on April 15, 2013, 8:51 AM
 */

#include <iostream>
#include <math.h>
#include <problems/boxcon/examples/dejong.hpp>
#include "boxconbnb.hpp"
#include "lipbounder.hpp"

/*
 * 
 */
main(int argc, char** argv) {
    DeJongProblem prob(3, -1, 4);
    LipBounder<double> lb(prob.mObj);
    lb.setLipConst(sqrt(2 * 4));
    BoxconBNB<double> bnb;
    bnb.addBounder(&lb);
    bnb.init(prob);
    long long steps = 100000;
    bnb.solve(steps);
    std::cout << "The solution is " << bnb.getIncumbent().mValue << " found in " << steps << " steps \n";
}

