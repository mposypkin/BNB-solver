/* 
 * File:   test2.hpp
 * Author: posypkin
 *
 * Test function 
 * Created on April 23, 2011, 7:56 PM
 */

#ifndef TEST2_HPP
#define	TEST2_HPP

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
        PolyUtil::fromString("x1 + x2", idents, *poly1);
        PolyLipObjective<double>* pobj1 = new PolyLipObjective<double>(poly1);
        Polynom<double>* poly2 = new Polynom<double>();
        PolyUtil::fromString("x1 - x2", idents, *poly2);
        PolyLipObjective<double>* pobj2 =  new PolyLipObjective<double>(poly2);
        Polynom<double>* poly3 = new Polynom<double>();
        PolyUtil::fromString("x1^2 + x2^2 - 2x1 - 2x2 + 1", idents, *poly3);
        PolyLipObjective<double>* pobj3 =  new PolyLipObjective<double>(poly3);
        Box<double> box(2);
        box.mA[0] = 0;
        box.mA[1] = 0;
        box.mB[0] = 1;
        box.mB[1] = 1;
        MultiData<double> md(box);
        md.mCriteria.push_back((LipzObjective<double>*) pobj1);
        md.mCriteria.push_back((LipzObjective<double>*) pobj2);
        md.mConstraints.push_back((LipzObjective<double>*) pobj3);
        return md;
    }

};





#endif	

