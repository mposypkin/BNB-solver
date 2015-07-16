/* 
 * File:   polymi.hpp
 * Author: medved
 *
 * Created on June 15, 2015, 6:34 PM
 */

#ifndef POLYBC_HPP
#define	POLYBC_HPP

#include <math.h>
#include <string>
#include <iostream>
#include <sstream>

#include <problems/nlp/common/nlpproblem.hpp>
#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/optlib/polyobjective.hpp>



/**
 * Generates an instance of box-constrained problem
 *  from polynomial, given as a string
 * 
 *  -d <= xi <= d, i =1,...,n
 */
class PolyBCFactory {
public:

    /**
     * Constructor 
     * @param n number of dimensions
     * @param d box size
     */
    PolyBCFactory(int n, int d, const std::string str) : 
    mN(n), mD(d)
    {
        mN = n;
        mD = d;
        mPobj = createPobj(str);
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

        /** Initializing bounding box **/
        Box<double> box(mN);
        initBox(box);
        prob->mBox = box;

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

    PolyObjective<double>* createPobj(const std::string& s) {
        std::map < std::string, int> idents;
        Polynom<double> *poly = new Polynom<double>();
        PolyUtil::fromString(s.c_str(), idents, *poly);
        PolynomTxt fmt;
        std::string ss = PolyUtil::toString(fmt, *poly);
        std::cout << "Polynom: " << ss << "\n";
        PolyObjective<double>* pobj = new PolyObjective<double>(poly);
        return pobj;
    }


    int mN;
    double mD;
    NlpProblem<double>* mProb;
    PolyObjective<double>* mPobj;
};


#endif	/* POLYMI_HPP */

