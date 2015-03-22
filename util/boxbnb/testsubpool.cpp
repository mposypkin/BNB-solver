/* 
 * File:   testsubpool.cpp
 * Author: medved
 *
 * Created on January 4, 2015, 2:14 PM
 */

#include <cstdlib>
#include <iostream>

#include "boxsubpool.hpp"

/*
 * Testing the subpool
 */
int main(int argc, char** argv) {

    BoxSub<double> bs1(3, 1);
    BoxSub<double> bs2(3, 2);
    BoxSub<double> bs3(3, 3);
    BoxSubPool<double> bspool;
    bspool.put(bs1);
    bspool.put(bs2);
    bspool.put(bs3);
    bspool.put(bs3);
    bspool.put(bs2);
    bspool.put(bs1);
    int prev = 0, cur = 0;
    while (!bspool.empty()) {
        BoxSub<double> bs = bspool.get();
        cur = bs.mDepth;
        BNB_ASSERT(cur >= prev);
        prev = cur;        
        std::cout << bspool.get().toString() << "\n";
    }
    bspool.setDirection(BoxSubPool<double>::Directions::TAKE_LOWER);
    bspool.put(bs1);
    bspool.put(bs2);
    bspool.put(bs3);
    bspool.put(bs3);
    bspool.put(bs2);
    bspool.put(bs1);
    
    while (!bspool.empty()) {
        cur = bspool.get().mDepth;
        BNB_ASSERT(cur <= prev);
        prev = cur;
        std::cout << bspool.get().toString() << "\n";
    }
    return 0;
}

