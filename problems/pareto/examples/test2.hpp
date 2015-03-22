/* 
 * File:   test1.hpp
 * Author: posypkin
 *
 * Test function taken from one of Karpenko's papers
 * Created on April 23, 2011, 7:56 PM
 */

#ifndef TEST2_HPP
#define	TEST2_HPP

#define POLY2_1 "x1"
#define POLY2_2 "x1^2 - 2x1 + x2 + 1"


#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/cover/poly/polylipobj.hpp>

template <typename FT> class Test2 {
public:

    /**
     * Fills in source data for the problem
     * @return the filled data
     */
    static MultiData<FT> fillInData() {
        std::map<std::string, int> idents;
        idents["x1"] = 0;
        idents["x2"] = 1;
        Polynom<double>* poly1 = new Polynom<double>();
        PolyUtil::fromString(POLY2_1, idents, *poly1);
        PolyLipObjective<double>* pobj1 = new PolyLipObjective<double>(poly1);
        Polynom<double>* poly2 = new Polynom<double>();
        PolyUtil::fromString(POLY2_2, idents, *poly2);
        PolyLipObjective<double>* pobj2 = new PolyLipObjective<double>(poly2);
        Box<double> box(2);
        box.mA[0] = 0;
        box.mA[1] = 0;
        box.mB[0] = 2;
        box.mB[1] = 2;
        MultiData<double> md(box);
        md.mCriteria.push_back((LipzObjective<double>*) pobj1);
        md.mCriteria.push_back((LipzObjective<double>*) pobj2);
        return md;

    }

};





#endif	/* TEST1_HPP */

