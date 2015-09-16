/* 
 * File:   test1.hpp
 * Author: posypkin
 *
 * Parametric test function for multiobjective optimization
 * Created on April 23, 2011, 7:56 PM
 */

#ifndef TESTP_HPP
#define	TESTP_HPP

#include <string>
#include <sstream>
#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <util/poly/polyutil.hpp>
#include <util/poly/polynomtxt.hpp>
#include <problems/cover/poly/polylipobj.hpp>

template <typename FT> class TestP {
public:

    /**
     * Fills in source data for the problem
     * @param n the dimensionality
     * @return the filled data
     */
    static MultiData<FT> fillInData(int n) {
        std::map<std::string, int> idents;
        for (int i = 0; i < n; i++) {
            std::ostringstream os;
            os << "x";
            os << i + 1;
            std::string s = os.str();
            idents[s] = i;
        }

        Polynom<double>* poly1 = new Polynom<double>();
        PolyUtil::fromString("x1", idents, *poly1);
        PolyLipObjective<double>* pobj1 = new PolyLipObjective<double>(poly1);

        std::ostringstream os;
        os << "-x1^2";
        for (int i = 2; i <= n; i++) {
            os << "+x" << i << "^2";
        }
        Polynom<double>* poly2 = new Polynom<double>();
        std::string s = os.str();
        printf("[%s]\n", s.c_str());
        PolyUtil::fromString(s.c_str(), idents, *poly2);
        PolyLipObjective<double>* pobj2 = new PolyLipObjective<double>(poly2);

        Box<double> box(n);
        for (int i = 0; i < n; i++) {
            box.mA[i] = 0;
            box.mB[i] = 1;
        }
        MultiData<double> md(box);
        md.mCriteria.push_back((LipzObjective<double>*) pobj1);
        md.mCriteria.push_back((LipzObjective<double>*) pobj2);
        return md;
    }

};





#endif	

