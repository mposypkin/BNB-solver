/* 
 * File:   nlputils.hpp
 * Author: mposypkin
 * 
 * Various utilities for non-linear programming
 *
 * Created on August 9, 2015, 2:52 PM
 */

#ifndef NLPUTILS_HPP
#define	NLPUTILS_HPP

#include <math.h>
#include <util/common/utilmacro.hpp>
#include "nlpproblem.hpp"

struct NLPUtils {

    /**
     * Computes the projection of a point x to a NLP problem
     * according to the box constraints and variable types
     * @param problem NLP problem for projection
     * @param x source point
     * @param y target point
     */
    template <class FT> static void project(const NlpProblem<FT>& problem, const FT* x, FT* y) {
        const int n = problem.mBox.mDim;
        for (int i = 0; i < n; i++) {
            FT r = BNBMAX(x[i], problem.mBox.mA[i]);
            r = BNBMIN(r, problem.mBox.mB[i]);
            FT u = r;
            if (!problem.mVariables.empty()) {
                if (problem.mVariables[i] == NlpProblem<FT>::VariableTypes::INTEGRAL) {
                    FT q = floor(r);
                    u = ((r - q) > 0.5) ? q + 1 : q;
                } else if (problem.mVariables[i] == NlpProblem<FT>::VariableTypes::BOOLEAN) {
                    u = (r <= 0.5) ? 0 : 1;
                }
            }
            y[i] = u;
        }
    }
};

#endif	/* NLPUTILS_HPP */

