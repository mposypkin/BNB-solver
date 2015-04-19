/* 
 * File:   testcomplement.cpp
 * Author: medved
 * Testing computing complement of a box
 * Created on April 17, 2015, 12:03 PM
 */

#include <vector>
#include <iostream>
#include "box.hpp"
#include "boxutils.hpp"

void printBoxList(std::vector< Box<double> > &v) {
    std::cout << "==============\n";
    for (auto o : v) {
        std::cout << BoxUtils::toString(o) << "\n";
    }
}

/*
 * 
 */
int main(int argc, char** argv) {

    Box<double> sbox(2);
    sbox.mA[0] = 0;
    sbox.mA[1] = 0;
    sbox.mB[0] = 4;
    sbox.mB[1] = 3;

    std::vector< Box<double> > bv;
    Box<double> cbox1(2);
    
    /** Test 1 **/
    cbox1.mA[0] = -1;
    cbox1.mA[1] = 1;
    cbox1.mB[0] = 5;
    cbox1.mB[1] = 2;

    BoxUtils::complement(sbox, cbox1, bv);
    printBoxList(bv);
    BNB_ASSERT(bv.size() == 2);
    BNB_ASSERT((bv[0].mA[0] == 0) && (bv[0].mA[1] == 0) && (bv[0].mB[0] == 4) && (bv[0].mB[1] == 1));
    BNB_ASSERT((bv[1].mA[0] == 0) && (bv[1].mA[1] == 2) && (bv[1].mB[0] == 4) && (bv[1].mB[1] == 3));

    /** Test 2 **/
    cbox1.mA[0] = 1;
    cbox1.mA[1] = 1;
    cbox1.mB[0] = 2;
    cbox1.mB[1] = 2;
    bv.clear();
    BoxUtils::complement(sbox, cbox1, bv);
    printBoxList(bv);
    BNB_ASSERT(bv.size() == 4);
    BNB_ASSERT((bv[0].mA[0] == 0) && (bv[0].mA[1] == 0) && (bv[0].mB[0] == 1) && (bv[0].mB[1] == 3));
    BNB_ASSERT((bv[1].mA[0] == 2) && (bv[1].mA[1] == 0) && (bv[1].mB[0] == 4) && (bv[1].mB[1] == 3));
    BNB_ASSERT((bv[2].mA[0] == 1) && (bv[2].mA[1] == 0) && (bv[2].mB[0] == 2) && (bv[2].mB[1] == 1));
    BNB_ASSERT((bv[3].mA[0] == 1) && (bv[3].mA[1] == 2) && (bv[3].mB[0] == 2) && (bv[3].mB[1] == 3));

    /** Test 3 **/
    cbox1.mA[0] = -2;
    cbox1.mA[1] = -2;
    cbox1.mB[0] = -1;
    cbox1.mB[1] = -1;
    bv.clear();
    BoxUtils::complement(sbox, cbox1, bv);
    printBoxList(bv);
    BNB_ASSERT(bv.size() == 1);
    BNB_ASSERT((bv[0].mA[0] == 0) && (bv[0].mA[1] == 0) && (bv[0].mB[0] == 4) && (bv[0].mB[1] == 3));    
    
    return 0;
}

