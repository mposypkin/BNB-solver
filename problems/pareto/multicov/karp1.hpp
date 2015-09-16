/* 
 * File:   karp1.hpp
 * Author: posypkin
 *
 * Test function taken from one of Karpenko's papers
 * Created on April 23, 2011, 7:56 PM
 */

#ifndef KARP1_HPP
#define	KARP1_HPP

#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <util/poly/polyutil.hpp>
#include <util/poly/polynomtxt.hpp>

template <typename FT> class Karp1 {
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
        PolyUtil::fromString("x1^2 + x2^2 - 10 x1 + 35", idents, *poly1);
        PolyLipObjective<double>* pobj1 = new PolyLipObjective<double>(poly1);
        Polynom<double>* poly2 = new Polynom<double>();
        PolyUtil::fromString("x1^2 + x2^2 - 10 x2 + 45", idents, *poly2);
        PolyLipObjective<double>* pobj2 =  new PolyLipObjective<double>(poly2);
        Box<double> box(2);
        box.mA[0] = 0;
        box.mA[1] = 0;
        box.mB[0] = 5;
        box.mB[1] = 5;
        MultiData<double> md(box);
        md.mCriteria.push_back((LipzObjective<double>*) pobj1);
        md.mCriteria.push_back((LipzObjective<double>*) pobj2);
        return md;
    }

};





#endif	/* KARP1_HPP */

