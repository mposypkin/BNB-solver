/* 
 * File:   pointtest.cpp
 * Author: posypkin
 *
 * Created on April 19, 2011, 5:42 AM
 */

#include <stdio.h>
#include <string>

#include "point.hpp"
#include "multipoint.hpp"
#include "mpsimpbag.hpp"

/*
 * Tests point and mutipoint operations
 */
int main(int argc, char** argv) {

    Point<double> p1(4);
    p1.mX[0] = 1;
    p1.mX[1] = 2;
    p1.mX[2] = 3;
    p1.mX[3] = 4;
    BNB_ASSERT(p1.toString() == "1 2 3 4");
    Point<double> p2(4);
    p2.mX[0] = 2;
    p2.mX[1] = 2;
    p2.mX[2] = 4;
    p2.mX[3] = 4;

    BNB_ASSERT(p1 <= p2);
    BNB_ASSERT(p2 >= p1);

    MultiPoint<double> mp1(4, 2);
    mp1.mParam.mX[0] = 1;
    mp1.mParam.mX[1] = 1;
    mp1.mParam.mX[2] = 1;
    mp1.mParam.mX[3] = 1;
    mp1.mCrit.mX[0] = 2;
    mp1.mCrit.mX[1] = 3;

    
    MultiPoint<double> mp2(4, 2);
    mp2.mParam.mX[0] = 1;
    mp2.mParam.mX[1] = 1;
    mp2.mParam.mX[2] = 1;
    mp2.mParam.mX[3] = 1;
    mp2.mCrit.mX[0] = 3;
    mp2.mCrit.mX[1] = 3;

    BNB_ASSERT(mp1 <= mp2);
    BNB_ASSERT(mp2 >= mp1);
    BNB_ASSERT(mp1.toString() == "<x>1 1 1 1</x><f>2 3</f>");

    MPSimpBag<double> bag;
    bag.put(mp1);
    bag.put(mp2);
    printf("[%s]\n", bag.toString().c_str());
    MultiPoint<double> mp = bag.get();
    printf("mp = %s\n", mp.toString().c_str());
}

