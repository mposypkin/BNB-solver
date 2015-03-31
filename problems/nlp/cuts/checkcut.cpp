/* 
 * File:   checkcut.cpp
 * Author: medved
 *
 * Created on March 24, 2015, 10:55 PM
 */

#include <vector>
#include <iostream>
#include "cututils.hpp"

int main(int argc, char** argv) {
    Box<double> box(2);
    box.mA[0] = -1;
    box.mA[1] = -1;
    box.mB[0] = 0;
    box.mB[1] = 0;

    Cut<double> cut;
    cut.mC.realloc(2);
    cut.mC[0] = -1.265;
    cut.mC[1] = -1.25;
    cut.mR = 1.31291;
    cut.mType = Cut<double>::CutType::INNER_BALL;
    
    std::vector< Box<double> > boxv;
    CutUtils<double>::ApplyCut(cut, box, boxv);
    std::cout << "Inner ball cut test\n";
    for (auto b : boxv) {
        std::cout << BoxUtils::toString(b);
        std::cout << "\n";
    }

    return 0;
}

