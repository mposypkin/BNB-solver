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
#include <problems/poly/polynom.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/optlib/polyobjective.hpp>

/**
 * Generates an instance of scaled mixed integer sum of square problems
 *  sum (a * xi - 1)^2 -> min, where a >= 2 a scale factor
 *  -1 <= xi <= d, i =1,...,n
 *  d = floor(C/a), where C is a parameter controlling the number
 *  of integral nodes in a mesh MN = C / 2
 */
class MISqSumFactory {
public:

    /**
     * Constructor 
     * @param n number of dimensions
     * @param mn number of nodes in a mesh by one side
     * @param a the scale factor
     */
    MISqSumFactory(int n, int mn, double a) {
        mN = n;
        mMN = mn;
        mA = a;
        mD = floor(2 * mn / a);
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

        prob->mVariables.reserve(mN);
        /** Initilizing variables **/
        for (int i = 0; i < mN; i++)
            prob->mVariables[i] = NlpProblem<double>::VariableTypes::INTEGRAL;

        /** Setup objective */
        prob->mObj = mPobj;
        return prob;
    }

    void initBox(Box<double> &box) {
        for (int i = 0; i < mN; i++) {
            box.mA[i] = -1;
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
            if(i != 0)
                os << " + ";
            os << mA * mA ;
            os << " x" << i << "^2";
        }
        for (int i = 0; i < mN; i++) {
            os << " - " << 2 * mA ;
            os << "x" << i ;
        }
        os << " + " << mN;
        return os.str();
    }

    int mN, mMN;
    double mA, mD;
    NlpProblem<double>* mProb;
    PolyObjective<double>* mPobj;
};

#endif	/* MISQSUMFACT_HPP */

