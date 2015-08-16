/* 
 * File:   nlprecstore.hpp
 * Author: medved
 * Record storage for generic mathematical (non-linear) programming
 * Created on April 11, 2015, 12:45 PM
 */

#ifndef NLPRECSTORE_HPP
#define	NLPRECSTORE_HPP

#include <problems/nlp/common/nlpproblem.hpp> 

#include "recstore.hpp"

/**
 * Record storage for unconstrained optimization
 */
template <class FT> class NlpRecStore : public RecStore <FT> {
public:

    struct Options {
        /**
         * Minimization problem
         */
        static const unsigned int MINIMIZE = 1;
        /**
         * Maximization problem
         */
        static const unsigned int MAXIMIZE = 1 << 1;
        /**
         * Never change the record
         */
        static const unsigned int NO_CHANGE = 1 << 2;
        /**
         * Always update the record
         */
        static const unsigned int ALWAYS_CHANGE = 1 << 3;
    };

    /**
     * Record storage properties
     */
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

    /**
     * The constructor
     * @param rec the initial record value
     * @param nlp nlp problem
     */
    NlpRecStore(FT rec, NlpProblem<FT> *nlp) {
        mRec = rec;
        mOptions = Options::MINIMIZE;
        mNLP = nlp;
        int n = mNLP->mBox.mDim;
        mX.alloc(n);
        mTX.alloc(n);
        mProp.mEqConsEps = 0;
        mProp.mIneqConsEps = 0;
    }

    /**
     * Retrieve the record value
     * @return the record value
     */
    FT getValue() const {
        return mRec;
    }

    /**
     * Retrieve the record vector 
     * @return record vector
     */
    const FT* getX() const {
        return mX;
    }

    /**
     * Update the record
     * @param rec new record value
     * @param x new record vector
     * @return true if record was updated
     */
    bool update(FT rec, const FT* x) {
        bool rv = false;
        if (project(x)) {
            FT v = mNLP->mObj->func(mTX);
            if (mOptions == Options::MINIMIZE) {
                if (v < mRec) {
                    rv = true;
                }
            } else if (mOptions == Options::MAXIMIZE) {
                if (v > mRec) {
                    rv = true;
                }
            } else if (mOptions == Options::NO_CHANGE) {
                rv = false;
            } else if (mOptions == Options::ALWAYS_CHANGE) {
                rv = true;
            }
            if (rv) {
                mRec = v;
                VecUtils::vecCopy(mX.size(), (FT*) mTX, (FT*) mX);
            }
        }
        return rv;
    }

    Options & getOptions() {
        return mOptions;
    }

    Properties& getProperties() {
        return mProp;
    }

private:

    bool project(const FT * x) {
        int n = mX.size();
        if (!mNLP->mVariables.empty()) {
            for (int i = 0; i < n; i++) {
                /**
                 * TMP solution - to be reworked completely
                 **/
                FT p = x[i];
                if (mNLP->mVariables[i] == NlpProblem<FT>::VariableTypes::INTEGRAL) {
                    int pl = p;
                    if (p - (FT) pl > 0.5) {
                        int pu = p + 1;
                        p = BNBMIN((FT) pu, mNLP->mBox.mB[i]);
                    } else {
                        p = BNBMAX((FT) pl, mNLP->mBox.mA[i]);
                    }
                }
                mTX[i] = p;
            }
        }
        bool ok = true;
        for (auto c : mNLP->mCons) {
            FT cv = c->mObjective->func(mTX);
            if (c->mType & Constraint<FT>::Types::INEQUALITY_CONSTRAINT) {
                if (cv > mProp.mIneqConsEps) {
                    ok = false;
                    break;
                }
            } else {
                if (BNBABS(cv) > mProp.mEqConsEps) {
                    ok = false;
                    break;
                }
            }
        }
        return ok;
    }

    unsigned int mOptions;
    FT mRec;
    SmartArrayPtr<FT> mX;
    SmartArrayPtr<FT> mTX;
    NlpProblem<FT> *mNLP;
    Properties mProp;

};


#endif	/* NLPRECSTORE_HPP */

