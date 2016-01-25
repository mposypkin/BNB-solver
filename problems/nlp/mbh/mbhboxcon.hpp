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
#include <problems/optlib/locboxoptim.hpp>
#include <util/common/smartptr.hpp>
#include <util/common/vec.hpp>
#include <util/common/bnbdebug.hpp>

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
    MBHBoxCon(const NlpProblem<FT>& prob, Perturber<FT>& perturber, int maxlochops, LocalBoxOptimizer<FT>* ls = NULL) :
    mProb(prob), mPert(perturber), mMaxLocalHops(maxlochops), mLS(ls) {
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
        mPert.reset();

        do {
            for (int i = 0; i < mMaxLocalHops; i++) {
                /*
                  if(i == 28)
                    bnbDebugVar = 1;
                 */ 
                mPert.perturb(y, z);
                if(mLS != NULL) {
                    FT v;
                    mLS->search(z, &v);
                    std::cout << "Local search returned " << v << "\n";
                }
                FT objv = mProb.mObj->func((FT*) z);
                if (objv < robjv) {
                    mPert.success(i, objv, robjv);
                    std::cout << "objv = " << objv << "\n";
                    robjv = objv;
                    VecUtils::vecCopy(n, (FT*) z, (FT*) y);
                    i = 0;
                }                
            }
            mPert.fail(mMaxLocalHops);
        } while (mPert.cont());

        VecUtils::vecCopy(n, (FT*) y, (FT*) x);
        return robjv;
    }

private:
    const NlpProblem<FT>& mProb;
    Perturber<FT>& mPert;
    LocalBoxOptimizer<FT>* mLS;
    int mMaxLocalHops;
};

#endif	/* MBHBOXCON_HPP */

