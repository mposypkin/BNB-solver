/* 
 * File:   mbh.hpp
 * Author: medved
 *
 * Created on August 3, 2015, 7:25 PM
 */

#ifndef MBHNLP_HPP
#define	MBHNLP_HPP

#include <problems/nlp/common/nlpproblem.hpp>
#include <util/common/smartptr.hpp>
#include <util/common/vec.hpp>
#include "perturber.hpp"

/**
 * Modification of the Monotonic Basin Hopping for 
 * Non-Linear Programming with exactly one equality constraint (use penalty wrappers 
 * if the number of constraints is larger)
 */
template <class FT> class MBHNlp {
public:

    /**
     * Constructor 
     * @param prob the NLP problem to resolve
     * @param perturber perturber
     * @param maxlochops maximal number of local hops
     * @param delta equality constraint tolerance
     */
    MBHNlp(const NlpProblem<FT>& prob,  Perturber<FT>& perturber, int maxlochops, FT delta) :
    mProb(prob), mPert(perturber),  mMaxLocalHops(maxlochops), mDelta(delta) {
        BNB_ASSERT(prob.mCons.size() == 1);
        BNB_ASSERT(prob.mCons[0]->mType == Constraint<FT>::Types::EQUALITY_CONSTRAINT);
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
        FT rconsv = mProb.mCons[0]->mObjective->func((FT*) y);

        for (int i = 0; i < mMaxLocalHops; i++) {
            mPert.perturb(y, z);
            FT objv = mProb.mObj->func((FT*) z);
            FT consv = mProb.mCons[0]->mObjective->func((FT*) z);
            if ((BNBABS(consv) <= mDelta) && (BNBABS(rconsv) <= mDelta)) {
                if (objv < robjv) {
                    robjv = objv;
                    rconsv = consv;
                    VecUtils::vecCopy(n, (FT*) z, (FT*) y);
                    i = 0;
                }
            } else if (BNBABS(consv) < BNBABS(rconsv)) {
                robjv = objv;
                rconsv = consv;
                VecUtils::vecCopy(n, (FT*) z, (FT*) y);
                i = 0;
            }
        }
        
        VecUtils::vecCopy(n, (FT*) y, (FT*) x);
        return robjv;
    }

private:
    const NlpProblem<FT>& mProb;
    Perturber<FT>& mPert;
    int mMaxLocalHops;
    FT mDelta;
};

#endif	/* MBH_HPP */

