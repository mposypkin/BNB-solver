/* 
 * File:   mbhboxcon.hpp
 * Author: medved
 *
 *  Version of monotonic basin hopping 
 *  for box constrained problems
 * 
 * Created on September 18, 2015, 8:28 AM
 */

#ifndef MBHBOXCON_HPP
#define	MBHBOXCON_HPP

#include <problems/nlp/common/nlpproblem.hpp>
#include <util/common/smartptr.hpp>
#include <util/common/vec.hpp>
#include "perturber.hpp"

/**
 * Modification of the Monotonic Basin Hopping for 
 * Non-Linear Programming with box constraints
 */
template <class FT> class MBHBoxCon {
public:

    /**
     * Constructor 
     * @param prob the NLP problem to resolve
     * @param perturber perturber
     * @param maxlochops maximal number of local hops
     */
    MBHBoxCon(const NlpProblem<FT>& prob, const Perturber<FT>& perturber, int maxlochops) :
    mProb(prob), mPert(perturber), mMaxLocalHops(maxlochops) {
        BNB_ASSERT(prob.mCons.empty());
    }

    /**
     * The search method
     * @param x on exit the feasible value
     * @return the value found
     */
    FT search(FT* x) {
        long long int nstep = 0;
        int n = mProb.mBox.mDim;
        SmartArrayPtr<FT> y(n);
        SmartArrayPtr<FT> z(n);
        VecUtils::vecCopy(n, x, (FT*) y);
        FT robjv = mProb.mObj->func((FT*) y);

        for (int i = 0; i < mMaxLocalHops; i++) {
            std::cout << "robjv = " << robjv << "\n";
            mPert.perturb(y, z);
            FT objv = mProb.mObj->func((FT*) z);
            if (objv < robjv) {
                robjv = objv;
                VecUtils::vecCopy(n, (FT*) z, (FT*) y);
                i = 0;
            }
        }

        VecUtils::vecCopy(n, (FT*) y, (FT*) x);
        return robjv;
    }

private:
    const NlpProblem<FT>& mProb;
    const Perturber<FT>& mPert;
    int mMaxLocalHops;
};

#endif	/* MBHBOXCON_HPP */

