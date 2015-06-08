/* 
 * File:   testmiboxsplitter.cpp
 * Author: medved
 *
 * Created on June 8, 2015, 7:19 PM
 */

#include <iostream>
#include "miboxsplitter.hpp"

/*
 * Test for Mixed-Integer Box Splitter
 */
int main(int argc, char** argv) {
    Box<double> box(2);
    box.mA[0] = 0;
    box.mB[0] = 9;
    box.mA[1] = -2;
    box.mB[1] = 1;
    std::vector<unsigned int> vart = {NlpProblem<double>::VariableTypes::GENERIC, NlpProblem<double>::VariableTypes::INTEGRAL};
    MIBoxSplitter<double> misplt(vart);
    
    std::vector< Box<double> > bv;
    misplt.split(box, bv);
    std::cout << "Resulting boxes:\n";    
    for(auto o : bv) {
        std::cout << BoxUtils::toString(o) << "\t";
    }
    std::cout << "\n";    
    BNB_ASSERT((bv[0].mB[1] == -1) && (bv[1].mA[1] == 0));

    vart[0] = NlpProblem<double>::VariableTypes::INTEGRAL;
    bv.clear();    
    misplt.split(box, bv);
    std::cout << "Resulting boxes:\n";    
    for(auto o : bv) {
        std::cout << BoxUtils::toString(o) << "\t";
    }
    std::cout << "\n";
    BNB_ASSERT((bv[0].mB[0] == 4) && (bv[1].mA[0] == 5));

    vart[1] = NlpProblem<double>::VariableTypes::BOOLEAN;
    bv.clear();    
    misplt.split(box, bv);
    std::cout << "Resulting boxes:\n";    
    for(auto o : bv) {
        std::cout << BoxUtils::toString(o) << "\t";
    }
    std::cout << "\n";
    BNB_ASSERT((bv[0].mB[1] == 0) && (bv[1].mA[1] == 1));
    
    return 0;
}

