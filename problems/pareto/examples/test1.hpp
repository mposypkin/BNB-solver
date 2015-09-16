/* 
 * File:   test1.hpp
 * Author: posypkin
 *
 * Test function taken from one of Karpenko's papers
 * Created on April 23, 2011, 7:56 PM
 */

#ifndef TEST1_HPP
#define	TEST1_HPP

#define POLY0 "x1 x2^2 - x2^2 + 1"
#define POLY1 "x1 x2^4-x2^4-1.3 x1 x2^3+1.3 x2^3+0.46 x1 x2^2 - 0.46 x2^2-0.048 x1 x2 + 0.048 x2 + 1"
#define POLY2 "x1 x2^3 - x2^3 - 1.2 x1 x2^2 + 1.2 x2^2 + 0.32 x1 x2 - 0.32 x2 + 1"
#define POLY3 "-10 x1 x2^2 + 10 x2^2 + 1.0 x1 x2 - 1.0 x2 + 1"

#define MINO1 "-0.5 x1^2 - 0.5 x2^2 + x1 - 2x2 + 3"
//#define MINO2 "- x1^2 - 0.5 x2^2 - 2x1 +  x2"
//#define MINO1 "x1 - 2x2"
#define MINO2 "- 2x1 +  x2 + 3"

#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <util/poly/polyutil.hpp>
#include <util/poly/polynomtxt.hpp>
#include <problems/cover/poly/polylipobj.hpp>

template <typename FT> class Test1 {
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
        PolyUtil::fromString(POLY0, idents, *poly1);
        PolyLipObjective<double>* pobj1 = new PolyLipObjective<double>(poly1);
        Polynom<double>* poly2 = new Polynom<double>();
        PolyUtil::fromString("x2", idents, *poly2);
        PolyLipObjective<double>* pobj2 = new PolyLipObjective<double>(poly2);
        Box<double> box(2);
        box.mA[0] = 0;
        box.mA[1] = 0;
        box.mB[0] = 1;
        box.mB[1] = 1;
        MultiData<double> md(box);
        md.mCriteria.push_back((LipzObjective<double>*) pobj1);
        md.mCriteria.push_back((LipzObjective<double>*) pobj2);
        return md;

    }

};





#endif	/* TEST1_HPP */

