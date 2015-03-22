/* 
 * File:   evt.hpp
 * Author: posypkin
 *
 * Created on June 13, 2012, 12:29 PM
 * Test function proposed by Yuri Evtushenko
 *
 * f^1(x, y) = x;
 * f^2(x, y) = min(|x - 1|, 1.5 - x) + y;
 *
 * 0 <= x <= 2, 0 <= y <= 2
 */

#ifndef EVT_HPP
#define	EVT_HPP

#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/optlib/polyobjective.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/cover/poly/polylipobj.hpp>

template <typename FT> class Crit1 : public LipzObjective<FT> {
public:
    /**
     * The objective function itself
     *
     * @param x argument
     *
     * @return function value
     *
     */
   FT func(const FT* x){
       return x[0];
   }

    /**
     * Calculate a lipzic constant for a function for a given rectangle
     *
     * @param a lower bound
     * @param b upper bound
     * @return Lipzic constant
     */
    FT getFuncLipzConstant(const FT* a, const FT* b) {
        return 1;
    }
};

template <typename FT> class Crit2 : public LipzObjective<FT> {
public:
    /**
     * The objective function itself
     *
     * @param x argument
     *
     * @return function value
     *
     */
   FT func(const FT* x){
#if 1
       static int count;
       printf("count = %d\n", ++ count);
#endif
       return BNBMIN(BNBABS(x[0] - 1), 1.5 - x[0]) + x[1] + 1;
   }

    /**
     * Calculate a lipzic constant for a function for a given rectangle
     *
     * @param a lower bound
     * @param b upper bound
     * @return Lipzic constant
     */
    FT getFuncLipzConstant(const FT* a, const FT* b) {
        return 1;
    }
};

/**
 * Example proposed by Yu.G. Evtushenko
 */
template <typename FT> class Evt {
public:

    /**
     * Fills in source data for the problem
     * @return the filled data
     */
    static MultiData<FT> fillInData() {
        Crit1<double>* obj1 = new Crit1<double>();
        Crit2<double>* obj2 = new Crit2<double>();
        Box<double> box(2);
        box.mA[0] = 0;
        box.mA[1] = 0;
        box.mB[0] = 2;
        box.mB[1] = 2;
        MultiData<double> md(box);
        md.mCriteria.push_back((LipzObjective<double>*) obj1);
        md.mCriteria.push_back((LipzObjective<double>*) obj2);
        return md;
    }
};

#endif	/* EVT_HPP */

