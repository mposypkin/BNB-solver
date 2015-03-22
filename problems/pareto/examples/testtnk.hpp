/* 
 * File:   testtnk.hpp
 * Author: posypkin
 *
 * TNK test
 *
 * Created on January 30, 2013, 3:11 PM
 */

#ifndef TESTTNK_HPP
#define	TESTTNK_HPP

#include <math.h>
#include <problems/optlib/objective.hpp>
#include <util/common/util.hpp>

#define MINX 0.00000001

class TNKCRI1 : public Objective<double> {
public:

    double func(const double* x) {
        return x[0];
    }

};

class TNKCRI2 : public Objective<double> {
public:

    double func(const double* x) {
        return x[1];
    }

};

class TNKCONS1 : public Objective<double> {
public:

    double func(const double* x) {
        return -x[0] * x[0] - x[1] * x[1] + 0.1 * cos(16 * atan(x[0] / (BNBMAX(x[1], MINX)))) + 1;
    }

};

class TNKCONS2 : public Objective<double> {
public:

    double func(const double* x) {
        return (x[0] - 0.5) * (x[0] - 0.5) + (x[1] - 0.5)*(x[1] - 0.5) - 0.5;
    }

};

class TestTNK {
public:

    static MultiData<double> fillInData() {
        static TNKCRI1 cri1;
        static TNKCRI2 cri2;
        static TNKCONS1 cons1;
        static TNKCONS2 cons2;
        Box<double> box(2);
        box.mA[0] = 0;
        box.mA[1] = 0;
        box.mB[0] = M_PI;
        box.mB[1] = M_PI;
        MultiData<double> md(box);
        md.mCriteria.push_back(&cri1);
        md.mCriteria.push_back(&cri2);
        md.mConstraints.push_back(&cons1);
        md.mConstraints.push_back(&cons2);
        return md;
    }
};


#endif	/* TESTTNK_HPP */

