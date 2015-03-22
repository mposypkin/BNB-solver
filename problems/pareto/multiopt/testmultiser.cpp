/* 
 * File:   testmultiser.cpp
 * Author: posypkin
 *
 * Created on September 5, 2011, 1:49 PM
 */



#include <util/box/box.hpp>
#include <util/box/boxutils.hpp>
#include <util/box/simplboxbag.hpp>
#include <util/box/boxser.hpp>
#include "multiser.hpp"

/*
 * 
 */
int main(int argc, char** argv) {
    BinarySerializer bs;
    Point<double> p(2);
    p.mX[0] = 1;
    p.mX[1] = 2;
    bs << p;
    MultiPoint<double> mp(2,2);
    mp.mParam.mX[0] = 1;
    mp.mParam.mX[1] = 2;
    mp.mCrit.mX[0] = 3;
    mp.mCrit.mX[1] = 4;
    bs << mp;
    Point<double> q(2);
    bs.reset();
    bs >> q;
    //rintf("%s\n", q.toString().c_str());
    BNB_ASSERT((p.mX[0] == q.mX[0]) && (p.mX[1] == q.mX[1]));
    MultiPoint<double> mq(2,2);
    bs >> mq;
    //printf("%s\n", mq.toString().c_str());
    BNB_ASSERT((mp.mParam.mX[0] == mq.mParam.mX[0]) && (mp.mParam.mX[1] == mq.mParam.mX[1]));
    BNB_ASSERT((mp.mCrit.mX[0] == mq.mCrit.mX[0]) && (mp.mCrit.mX[1] == mq.mCrit.mX[1]));
    MultiPoint<double> mp1(2,3);
    mp1.mParam.mX[0] = 0;
    mp1.mParam.mX[1] = 1;
    mp1.mCrit.mX[0] = 3;
    mp1.mCrit.mX[1] = 4;
    mp1.mCrit.mX[2] = 5;
    MultiPoint<double> mp2(2,3);
    mp2.mParam.mX[0] = 10;
    mp2.mParam.mX[1] = 11;
    mp2.mCrit.mX[0] = 5;
    mp2.mCrit.mX[1] = 4;
    mp2.mCrit.mX[2] = 3;
    MPSimpBag<double> mpsbag;
    mpsbag.put(mp1);
    mpsbag.put(mp2);
    bs.reset();
    bs << mpsbag;
    bs.reset();
    MPSimpBag<double> mpsbagn;
    bs >> mpsbagn;
    //printf("%s\n", mpsbagn.toString().c_str());
    BNB_ASSERT(mpsbagn.size() == 2);
    bs.reset();

    Box<double> bx1(2);
    bx1.mA[0] = 1;
    bx1.mA[1] = 2;
    bx1.mB[0] = 3;
    bx1.mB[1] = 4;
    bs << bx1;
    Box<double> bx2(2);
    bs.reset();
    bs >> bx2;
    //printf("%s\n", BoxUtils::toString(bx2).c_str());
    BNB_ASSERT(bx1.mA[0] == bx2.mA[0]);
    BNB_ASSERT(bx1.mA[1] == bx2.mA[1]);
    BNB_ASSERT(bx1.mB[0] == bx2.mB[0]);
    BNB_ASSERT(bx1.mB[1] == bx2.mB[1]);
    
    SimpBoxBag<double> sb;
    sb.put(bx1);
    sb.put(bx2);
    bs.reset();
    bs << sb;
    bs.reset();
    SimpBoxBag<double> sb1;
    bs >> sb1;
    //printf("sb1: %s\n", sb1.toString().c_str());
    BNB_ASSERT(sb1.size() == 2);
    return 0;
}

