/* 
 * File:   misqsum.hpp
 * Author: medved
 *
 * Created on May 5, 2015, 8:57 AM
 */

#ifndef MISQSUMFACT_HPP
#define	MISQSUMFACT_HPP

#include <math.h>
#include <string>
#include <iostream>
#include <sstream>

#include <problems/nlp/common/nlpproblem.hpp>
#include <util/poly/polynom.hpp>
#include <util/poly/polynomtxt.hpp>
#include <util/poly/polyutil.hpp>
#include <problems/optlib/polyobjective.hpp>

/**
 * Generates an instance of scaled mixed integer sum of square problems
 *  x1^2 - x2^2 + x3^2 - ...
 *  -d <= xi <= d, i =1,...,n
 */
class MISqSumFactory {
public:

    /**
     * Constructor 
     * @param n number of dimensions
     * @param d box size
     */
    MISqSumFactory(int n, int d) {
        mN = n;
        mD = d;
        mPobj = createPobj();
        mProb = createProb();
    }

    /**
     * Retrieve NLP problem (lazy style)
     * @return nlp problem
     */
    NlpProblem<double>* getProb() {
        return mProb;
    }




private:

    NlpProblem<double>* createProb() {
        NlpProblem<double>* prob = new NlpProblem<double>();

        /** Initilizing bounding box **/
        Box<double> box(mN);
        initBox(box);
        prob->mBox = box;

        prob->mVariables.resize(mN);
        
        /** Initilizing variables **/
        for (int i = 0; i < mN; i++)
            prob->mVariables[i] = NlpProblem<double>::VariableTypes::INTEGRAL;

        /** Setup objective */
        prob->mObj = mPobj;
        return prob;
    }

    void initBox(Box<double> &box) {
        for (int i = 0; i < mN; i++) {
            box.mA[i] = -mD;
            box.mB[i] = mD;
        }
    }

    PolyObjective<double>* createPobj() {
        std::map < std::string, int> idents;
        Polynom<double> *poly = new Polynom<double>();
        PolyUtil::fromString(getPolyString().c_str(), idents, *poly);
        PolynomTxt fmt;
        std::string ss = PolyUtil::toString(fmt, *poly);
        std::cout << "Polynom: " << ss << "\n";
        PolyObjective<double>* pobj = new PolyObjective<double>(poly);
        return pobj;
    }

    std::string getPolyString() {
        std::ostringstream os;
        for (int i = 0; i < mN; i++) {
            if(i != 0) {                
                os << ((i % 2) ? " - " : "+");
            }
            os << " x" << i << "^2";
        }
        return os.str();
    }

    int mN;
    double mD;
    NlpProblem<double>* mProb;
    PolyObjective<double>* mPobj;
};

#endif	/* MISQSUMFACT_HPP */

