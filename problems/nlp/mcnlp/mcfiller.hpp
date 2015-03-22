/* 
 * File:   mcfiller.hpp
 * Author: medved
 *
 * Created on October 20, 2014, 10:56 AM
 * 
 * Simple Monte-Carlo filler
 */

#ifndef MCFILLER_HPP
#define	MCFILLER_HPP

#include <stdlib.h>
#include "mcnlp.hpp"

template <class FT> class MCFiller : public MCNlp<FT>::SpaceFiller {
public:

    struct Properties {
        /**
         * Epsilon for equality contraints
         */
        FT mEqConsEps;

        /**
         * Epsilon for inequlity constraints
         */
        FT mIneqConsEps;
    };

    MCFiller(NlpProblem<FT>* prob) : mProb(prob) {
        mProp.mEqConsEps = 0;
        mProp.mIneqConsEps = 0;
    }

    bool next(FT *x) {
        int n = mProb->mBox.mDim;
        SmartArrayPtr<FT> xk(n);
        //std::cout << "n = " << n << " \n";
        for (;;) {
            bool ok = true;
            for (int i = 0; i < n; i++) {
                FT r = (FT) rand() / (FT) RAND_MAX;
                //std::cout << "r = " << r << "," << mProb->mBox.mB[i] << "," << mProb->mBox.mA[i] << "\n";
                double p = mProb->mBox.mA[i] + (mProb->mBox.mB[i] - mProb->mBox.mA[i]) * r;
                if (mProb->mVariables[i] == NlpProblem<FT>::VariableTypes::INTEGRAL) {
                    int pl = p;
                    if (p - (FT) pl > 0.5) {
                        int pu = p + 1;
                        p = BNBMIN((FT) pu, mProb->mBox.mB[i]);
                    } else
                        p = pl;
                }
                xk[i] = p;
                //std::cout << "xk[" << i << "] = " << xk[i] << "\n";
            }
            for (auto c : mProb->mCons) {
                FT cv = c->mObjective->func(xk);
                //std::cout << "constraint value: " << cv << "\n";
                if (c->mType & Constraint<FT>::Types::INEQUALITY_CONSTRAINT) {
                    if (cv > mProp.mIneqConsEps) {
                        ok = false;
                        break;
                    }
                } else {
                    if (BNBABS(cv) > mProp.mIneqConsEps) {
                        ok = false;
                        break;
                    }                    
                }
            }
            if (ok == true) {
                VecUtils::vecCopy(n, (FT*) xk, x);
                break;
            }
        }
        return true;
    }
    
    Properties& getProperties() {
        return mProp;
    }
    
private:

    NlpProblem<FT>* mProb;
    Properties mProp;

};


#endif	/* MCFILLER_HPP */

