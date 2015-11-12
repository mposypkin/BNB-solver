/* 
 * File:   adaptpert.hpp
 * Author: medved
 *
 * Created on August 4, 2015, 5:16 PM
 */

#ifndef ADAPTPERT_HPP
#define	ADAPTPERT_HPP

#include <stdlib.h>
#include <util/common/random.hpp>
#include <util/common/utilmacro.hpp>
#include <problems/nlp/common/nlpproblem.hpp>
#include "perturber.hpp"

/**
 * Perturber based on dynamically changing neighbourhood
 */
template <class FT> class AdaptPerturber : public Perturber <FT> {
public:

    /**
     * Adaptive perturber parameters
     */
    struct Params {
        /**
         * Initial value of R
         */
        const FT mRinit;
        /**
         * Minimal value of scale
         */
        const FT mRmin;
        /** 
         * Maximal value of scale
         */
        const FT mRmax;
        /**
         * Decremental multiplier
         */
        const FT mDec;
        /**
         * Increment multiplier
         */
        const FT mInc;
    };

    /**
     * Constructor 
     * @param problem NLP problem to solve
     * @param vicinity - to generate local perturbations, vicinity is centered at zero, e.g. use [-1,1] to generate symmetric perturbations
     */
    AdaptPerturber(const NlpProblem<FT>& problem, const Box<FT>& vicinity, const Params& params) :
    mProblem(problem), mVicinity(vicinity), mParams(params), mR(params.mRinit){
    }

    /**
     * Generates perturbed point
     * @param x source point
     * @param y resulting (perturbed) point
     */
    void perturb(const FT * x, FT * y) {
        int n = mProblem.mBox.mDim;
        for (int i = 0; i < n; i++) {
            FT r = BnbRandom::get(x[i] + mR * mVicinity.mA[i], x[i] + mR * mVicinity.mB[i]);
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

    void reset() {
        mR = mParams.mRinit;
    }
    
    void fail(int ntries) {
        mR *= mParams.mDec;
        std::cout << "Decreased mR := " << mR << "\n";
    }

    void success(int ntries, FT pv, FT nv) {
        mR = BNBMIN(mParams.mRmax, mR * mParams.mInc);
        //std::cout << "Increased mR := " << mR << "\n";
    }

    bool cont() const {
        return mR >= mParams.mRmin;
    }
    
  
private:

    const NlpProblem<FT>& mProblem;
    const Box<FT>& mVicinity;
    const Params mParams;
    FT mR;
};

#endif	/* STATICPERT_HPP */

