/* 
 * File:   dejong.hpp
 * Author: mikdebian
 *
 * Created on April 15, 2013, 10:56 AM
 */

#ifndef DEJONG_HPP
#define	DEJONG_HPP

#include <problems/optlib/objective.hpp>
#include <problems/boxcon/common/boxconproblem.hpp>
#include <util/common/vec.hpp>


class DeJongObjective : public Objective <double> {
    public:
        DeJongObjective(int n)  {
            Objective<double>::setDim(n);
        }
        
        double func(const double* x) {
            int n = getDim();
            return VecUtils::vecNormTwoSqr(n, x);
        }
};

struct DeJongProblem : public BoxconProblem <double> {
    typedef BoxconProblem <double> BASE;
    
    DeJongProblem(int dim, double a, double b) {
        Box<double> box(dim);
        for(int i = 0; i < dim; i ++) {
            box.mA[i] = a;
            box.mB[i] = b;
        }
        BASE::mBox = box;
        BASE::mObj = new DeJongObjective(dim);        
    }
    
    ~DeJongProblem() {delete BASE::mObj;}
    
};

#endif	/* DEJONG_HPP */

