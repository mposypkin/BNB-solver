/* 
 * File:   testmcnlp.cpp
 * Author: medved
 *
 * Created on October 19, 2014, 7:53 PM
 */
#include <iostream>
#include "mcnlp.hpp"
#include "mcfiller.hpp"

class MyStopper : public MCNlp<double>::Stopper {
    public:
        MyStopper(long long int maxstep) : mMaxStep (maxstep) {            
        }
        
    bool stop(double fv, long long int step) {
        //std::cout << "step: " << step << ", fv: " << fv << std::endl;
        if(step > mMaxStep)
            return true;
        else
            return false;
    }
    private:
        long long int mMaxStep;
};

class Obj : public Objective <double>{
    public:
    Obj() {        
        /**
         * Setup dimension (number of vars)
         */
        Objective<double>::setDim(2);
    }
    
    /**
     * Computes objective function value x0^2 + x1 - 4 <= 0
     * @param x parametes
     * @return constraint value
     */
    double func(const double* x) {        
        return x[0] + x[1] ;
    }
    
};

class ObjCons1 : public Objective <double>{
    public:
    ObjCons1() {        
        /**
         * Setup dimension (number of vars)
         */
        Objective<double>::setDim(2);
    }
    
    /**
     * Computes constraint value x0^2 + x1 - 4 <= 0
     * @param x parametes
     * @return constraint value
     */
    double func(const double* x) {        
        return 3.5 - x[0]*x[0] - x[1];
    }
    
};

class ObjCons2 : public Objective <double>{
    public:
    ObjCons2() {        
        /**
         * Setup dimension (number of vars)
         */
        Objective<double>::setDim(2);
    }
    
    /**
     * Computes constraint value x0^2 + x1 - 4 <= 0
     * @param x parametes
     * @return constraint value
     */
    double func(const double* x) {        
        return 2 * x[0] - x[1];
    }
    
};


void initbox(Box<double> &box) {
    box.mA[0] = 0;
    box.mA[1] = 0;
    box.mB[0] = 4;
    box.mB[1] = 8;
}




/*
 * 
 */
int main(int argc, char** argv) {
    Box<double> box(2);
    initbox(box);
    Obj obj;
    ObjCons1 objcons1;
    Constraint<double> cons1(&objcons1, Constraint<double>::Types::INEQUALITY_CONSTRAINT);
    ObjCons2 objcons2;
    Constraint<double> cons2(&objcons2, Constraint<double>::Types::INEQUALITY_CONSTRAINT | Constraint<double>::Types::LINEAR);
    
    NlpProblem<double> prob;
    prob.mBox = box;
    prob.mCons.push_back(&cons1);
    prob.mCons.push_back(&cons2);
    prob.mObj = &obj;
    prob.mVariables.alloc(2);
    //prob.mVariables[0] = NlpProblem<double>::VariableTypes::GENERIC;
    prob.mVariables[0] = NlpProblem<double>::VariableTypes::INTEGRAL;
    //prob.mVariables[1] = NlpProblem<double>::VariableTypes::GENERIC;
    prob.mVariables[1] = NlpProblem<double>::VariableTypes::INTEGRAL;
    MCFiller<double> filler(&prob);
    MyStopper stopper(10000);
    MCNlp<double> mcnlp(&prob, &stopper, &filler);
    double x[2];
    double fv = mcnlp.search(x);
    //std::cout << "fv = " << fv << ", ";
    //VecUtils::vecPrint(2, x);
    if((x[0] != 1) || (x[1] != 3)) 
        BNB_ERROR_REPORT("Wrong result\n");
    return 0;
}

