/* 
 * File:   staticpert.hpp
 * Author: medved
 *
 * Created on August 4, 2015, 5:16 PM
 */

#ifndef STATICPERT_HPP
#define	STATICPERT_HPP

#include <stdlib.h>
#include <util/common/random.hpp>
#include <util/common/utilmacro.hpp>
#include <problems/nlp/common/nlpproblem.hpp>
#include "perturber.hpp"

/**
 * Simplest perturber based on static neighbourhood
 */
template <class FT> class StaticPerturber : public Perturber <FT> {
public:

    /**
     * Constructor 
     * @param problem NLP problem to solve
     * @param vicinity - to generate local perturbations, vicinity is centered at zero, e.g. use [-1,1] to generate symmetric perturbations
     */
    StaticPerturber(const NlpProblem<FT>& problem, const Box<FT>& vicinity) :
    mProblem(problem), mVicinity(vicinity) {
    }

    /**
     * Generates perturbed point
     * @param x source point
     * @param y resulting (perturbed) point
     */
    void perturb(const FT * x, FT * y) const {
        int n = mProblem.mBox.mDim;
        for (int i = 0; i < n; i++) {
            FT r = BnbRandom::get(x[i] + mVicinity.mA[i], x[i] + mVicinity.mB[i]);
            FT u;
            if (!mProblem.mVariables.empty()) {
                if (mProblem.mVariables[i] == NlpProblem<FT>::VariableTypes::GENERIC) {
                    u = r;
                } else if (mProblem.mVariables[i] == NlpProblem<FT>::VariableTypes::INTEGRAL) {
                    FT p = floor(r);
                    u = ((r - p) > 0.5) ? p + 1 : p;
                } else if (mProblem.mVariables[i] == NlpProblem<FT>::VariableTypes::BOOLEAN) {
                    u = (r > 0.5) ? 1 : 0;
                } else {
                    BNB_ERROR_REPORT("Unknown variable type");
                }
            } else {
                u = r;
            }
            u = BNBMAX(u, mProblem.mBox.mA[i]);
            u = BNBMIN(u, mProblem.mBox.mB[i]);
            y[i] = u;
        }

    }

private:


    const NlpProblem<FT>& mProblem;
    const Box<FT>& mVicinity;
};

#endif	/* STATICPERT_HPP */

