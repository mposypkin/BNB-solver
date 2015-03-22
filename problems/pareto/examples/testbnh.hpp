/* 
 * File:   test1.hpp
 * Author: posypkin
 *
 * Test function taken from one of Karpenko's papers
 * Created on April 23, 2011, 7:56 PM
 */

#ifndef TESTBNH_HPP
#define	TESTBNH_HPP

#define BNH_OBJ1 "4 x1^2 + 4 x2^2"
#define BNH_OBJ2 "x1^2 + x2^2 - 10 x1 - 10 x2 + 50"
#define BNH_CONS1 "x1^2 - 10 x1 + x2^2"
#define BNH_CONS2 "-x1^2 - x2^2 + 16 x1 - 6 x2 - 65.3"

#include <vector>
#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/cover/poly/polylipobj.hpp>

template <typename FT> class TestBNH {
public:

    /**
     * Fills in source data for the problem
     * @param crit vector of polynomial criteria
     * @param cons vector of polynomila constraints
     * @return the filled data
     */
    static MultiData<FT> fillInData(std::vector< Polynom <FT>* > &crit, std::vector< Polynom <FT>* > &cons) {
        std::map<std::string, int> idents;
        idents["x1"] = 0;
        idents["x2"] = 1;

        Polynom<double>* p1 = new Polynom<double>();
        PolyUtil::fromString(BNH_OBJ1, idents, *p1);
        PolyLipObjective<double>* obj1 = new PolyLipObjective<double>(p1);

        Polynom<double>* p2 = new Polynom<double>();
        PolyUtil::fromString(BNH_OBJ2, idents, *p2);
        PolyLipObjective<double>* obj2 = new PolyLipObjective<double>(p2);

        Polynom<double>* p3 = new Polynom<double>();
        PolyUtil::fromString(BNH_CONS1, idents, *p3);
        PolyLipObjective<double>* cons1 = new PolyLipObjective<double>(p3);

        Polynom<double>* p4 = new Polynom<double>();
        PolyUtil::fromString(BNH_CONS2, idents, *p4);
        PolyLipObjective<double>* cons2 = new PolyLipObjective<double>(p4);

        Box<double> box(2);

        box.mA[0] = 0;
        box.mA[1] = 0;
        box.mB[0] = 5;
        box.mB[1] = 3;

        MultiData<double> md(box);
        crit.push_back(p1);
        crit.push_back(p2);
        cons.push_back(p3);
        cons.push_back(p4);

        md.mCriteria.push_back((LipzObjective<double>*) obj1);
        md.mCriteria.push_back((LipzObjective<double>*) obj2);
        md.mConstraints.push_back(((LipzObjective<double>*)cons1));
        md.mConstraints.push_back(((LipzObjective<double>*)cons2));
        return md;
    }

};





#endif	/* TESTBNH_HPP */

