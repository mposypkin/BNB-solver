/* 
 * File:   test1.hpp
 * Author: posypkin
 *
 * Test function taken from one of Karpenko's papers
 * Created on April 23, 2011, 7:56 PM
 */

#ifndef TESTOSY_HPP
#define	TESTOSY_HPP

#define OBJ1 "-x5^2 + 2 x5 - x4^2 + 8 x4 - x3^2 + 2 x3 - x2^2 + 4 x2 - 25 x1^2 + 100 x1 - 122"
#define OBJ2 "x1^2 + x2^2 + x3^2 + x4^2 + x5^2 + x6^2"
#define CONS1 "-x1 - x2 + 2"
#define CONS2 "x1 + x2 - 6"
#define CONS3 "x2 - x1 - 2"
#define CONS4 "x1 - 3 x2 - 2"
#define CONS5 "x4 + x3^2 - 6 x3 + 5"
#define CONS6 "-x5^2 + 6 x5 - x6 -5"

#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <util/poly/polyutil.hpp>
#include <util/poly/polynomtxt.hpp>
#include <problems/cover/poly/polylipobj.hpp>

template <typename FT> class TestOSY {
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
        idents["x3"] = 2;
        idents["x4"] = 3;
        idents["x5"] = 4;
        idents["x6"] = 5;
        
        Polynom<double>* p1 = new Polynom<double>();
        PolyUtil::fromString(OBJ1, idents, *p1);
        PolyLipObjective<double>* obj1 = new PolyLipObjective<double>(p1);

        Polynom<double>* p2 = new Polynom<double>();
        PolyUtil::fromString(OBJ2, idents, *p2);
        PolyLipObjective<double>* obj2 = new PolyLipObjective<double>(p2);

        Polynom<double>* p3 = new Polynom<double>();
        PolyUtil::fromString(CONS1, idents, *p3);
        PolyLipObjective<double>* cons1 = new PolyLipObjective<double>(p3);
        
        Polynom<double>* p4 = new Polynom<double>();
        PolyUtil::fromString(CONS2, idents, *p4);
        PolyLipObjective<double>* cons2 = new PolyLipObjective<double>(p4);

        Polynom<double>* p5 = new Polynom<double>();
        PolyUtil::fromString(CONS3, idents, *p5);
        PolyLipObjective<double>* cons3 = new PolyLipObjective<double>(p5);

        Polynom<double>* p6 = new Polynom<double>();
        PolyUtil::fromString(CONS4, idents, *p6);
        PolyLipObjective<double>* cons4 = new PolyLipObjective<double>(p6);
        
        Polynom<double>* p7 = new Polynom<double>();
        PolyUtil::fromString(CONS5, idents, *p7);
        PolyLipObjective<double>* cons5 = new PolyLipObjective<double>(p7);

        Polynom<double>* p8 = new Polynom<double>();
        PolyUtil::fromString(CONS6, idents, *p8);
        PolyLipObjective<double>* cons6 = new PolyLipObjective<double>(p8);


        Box<double> box(6);
        
        box.mA[0] = 0; box.mA[1] = 0; box.mA[2] = 1;
        box.mA[3] = 0; box.mA[4] = 1; box.mA[5] = 0;
        box.mB[0] = 10; box.mB[1] = 10; box.mB[2] = 5;
        box.mB[3] = 6; box.mB[4] = 5; box.mB[5] = 10;

        crit.push_back(p1);
        crit.push_back(p2);
        cons.push_back(p3);
        cons.push_back(p4);
        cons.push_back(p5);
        cons.push_back(p6);
        cons.push_back(p7);
        cons.push_back(p8);

        MultiData<double> md(box);
        md.mCriteria.push_back((LipzObjective<double>*) obj1);
        md.mCriteria.push_back((LipzObjective<double>*) obj2);
        md.mConstraints.push_back(((LipzObjective<double>*)cons1));
        md.mConstraints.push_back(((LipzObjective<double>*)cons2));
        md.mConstraints.push_back(((LipzObjective<double>*)cons3));
        md.mConstraints.push_back(((LipzObjective<double>*)cons4));
        md.mConstraints.push_back(((LipzObjective<double>*)cons5));
        md.mConstraints.push_back(((LipzObjective<double>*)cons6));
        return md;
    }

};





#endif	/* TESTOSY_HPP */

