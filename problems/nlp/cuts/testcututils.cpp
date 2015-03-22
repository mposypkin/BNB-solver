/* 
 * File:   testcututils.cpp
 * Author: medved
 *
 * Created on January 20, 2015, 8:20 PM
 */

#include <vector>
#include <iostream>
#include "cututils.hpp"

void initBox(Box<double> &box) {
    box.mA[0] = 0;
    box.mA[1] = 0;
    box.mB[0] = 12;
    box.mB[1] = 6;
}

/*
 * 
 */
int main(int argc, char** argv) {
    Box<double> box(2);
    initBox(box);
    Cut<double> cut;
    cut.mC.realloc(2);
    cut.mC[0] = 6;
    cut.mC[1] = 3;
    cut.mR = 10;
    cut.mType = Cut<double>::CutType::INNER_BALL;
    BNB_ASSERT(CutUtils<double>::isIn(cut, box) == true);
    cut.mR = 5;
    BNB_ASSERT(CutUtils<double>::isIn(cut, box) == false);
    std::vector< Box<double> > boxv;
    CutUtils<double>::ApplyCut(cut, box, boxv);
    std::cout << "Inner ball cut test\n";
    for (auto b : boxv) {
        std::cout << BoxUtils::toString(b);
        std::cout << "\n";
    }
    BNB_ASSERT(boxv.size() == 2);
    BNB_ASSERT((boxv[0].mA[0] == 0) && (boxv[0].mA[1] == 0) && (boxv[0].mB[0] == 2) && (boxv[0].mB[1] == 6));
    BNB_ASSERT((boxv[1].mA[0] == 10) && (boxv[1].mA[1] == 0) && (boxv[1].mB[0] == 12) && (boxv[1].mB[1] == 6));

    cut.mC[0] = 20;
    cut.mC[1] = 40;
    cut.mType = Cut<double>::CutType::OUTER_BALL;
    BNB_ASSERT(CutUtils<double>::isIn(cut, box) == true);

    cut.mC[0] = 6;
    cut.mC[1] = 3;
    cut.mR = 4;
    BNB_ASSERT(CutUtils<double>::isIn(cut, box) == false);

    boxv.clear();
    CutUtils<double>::ApplyCut(cut, box, boxv);
    std::cout << "Outer ball cut test\n";
    for (auto b : boxv) {
        std::cout << BoxUtils::toString(b);
        std::cout << "\n";
    }
    BNB_ASSERT(boxv.size() == 1);
    BNB_ASSERT((boxv[0].mA[0] == 2) && (boxv[0].mA[1] == 0) && (boxv[0].mB[0] == 10) && (boxv[0].mB[1] == 6));

    
    cut.mR = 1;
    cut.mC[0] = 2;
    cut.mC[1] = 1;
    cut.mType = Cut<double>::CutType::LINEAR;
    BNB_ASSERT(CutUtils<double>::isIn(cut, box) == true);
    cut.mR = -8;
    BNB_ASSERT(CutUtils<double>::isIn(cut, box) == false);
    boxv.clear();
    CutUtils<double>::ApplyCut(cut, box, boxv);
    std::cout << "Linear cut test\n";
    for (auto b : boxv) {
        std::cout << BoxUtils::toString(b);
        std::cout << "\n";
    }
    BNB_ASSERT(boxv.size() == 1);
    BNB_ASSERT((boxv[0].mA[0] == 0) && (boxv[0].mA[1] == 0) && (boxv[0].mB[0] == 4) && (boxv[0].mB[1] == 6));

    return 0;
}

