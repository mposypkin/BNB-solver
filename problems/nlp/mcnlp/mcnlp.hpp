/* 
 * File:   mcnlp.hpp
 * Author: medved
 * Monte-Carlo (or other space exploring) techniques for global search
 * for NLP problems
 */

#ifndef MCNLP_HPP
#define	MCNLP_HPP

#include <util/common/vec.hpp>
#include <problems/nlp/common/nlpproblem.hpp>

template <class FT> class MCNlp {
public:

    /**
     * Determines when to stop
     */
    class Stopper {
    public:
        /**
         * Decides when to stop
         * @param fv current function value
         * @param step current step number
         * @return 
         */
        virtual bool stop(FT fv, long long int step) = 0;
    };

    /**
     * Space filler 
     * class
     */
    class SpaceFiller {
    public:
        /**
         * Generates new point
         * @param x new point
         * @return true if new point was generated, false otherwise
         */
        virtual bool next(FT *x) = 0;
    };


    MCNlp(NlpProblem<FT>* prob, Stopper* stopper, SpaceFiller* filler) :
    mProb(prob), mStopper(stopper), mFiller(filler) {

    }

    /**
     * The search method
     * @param x on exit the feasible value
     * @return the value found
     */
    FT search(FT* x) {
        FT fv = 0;
        int n = mProb->mBox.mDim;
        SmartArrayPtr<FT> xk(n);
        
        bool entry = true;
        long long int nstep = 0;
        for (;;) {
            nstep ++;
            if(!mFiller->next(xk)) 
                break;
            FT fvn = mProb->mObj->func(xk);
            if (entry || (fvn < fv)) {
                entry = false;
                fv = fvn;
                VecUtils::vecCopy(n, (FT*) xk, x);
            }
            if (mStopper->stop(fv, nstep))
                break;
        }
        return fv;
    }

private:
    NlpProblem<FT>* mProb;
    Stopper* mStopper;
    SpaceFiller* mFiller;


};

#endif	/* MCNLP_HPP */

