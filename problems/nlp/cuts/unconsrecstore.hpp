/* 
 * File:   unconsrecstore.hpp
 * Author: medved
 * 
 * Record storage for unconstrained optimization
 *
 * Created on January 31, 2015, 9:37 PM
 */

#ifndef UNCONSRECSTORE_HPP
#define	UNCONSRECSTORE_HPP

#include <util/common/smartptr.hpp>
#include <util/common/vec.hpp>
#include "recstore.hpp"

/**
 * Record storage for unconstrained optimization
 */
template <class FT> class UnconsRecStore : public RecStore <FT> {
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
     * The constructor
     * @param rec the initial record value
     * @param n dimension
     */
    UnconsRecStore(FT rec, int n) {
        mRec = rec;
        mOptions = Options::MINIMIZE;
        mX.alloc(n);
    }

    /**
     * Retrieve the record value
     * @return the record value
     */
    FT getValue() const {
        return mRec;
    }

    SmartArrayPtr<FT>& getX() const {
        return mX;
    }

    /**
     * Update the record
     * @param rec new record value
     * @param x new record vector
     * @return true if record was updated
     */
    bool update(FT rec, FT* x) {
        bool rv = false;

        if (mOptions == Options::MINIMIZE) {
            if (rec < mRec) {
                rv = true;
                mRec = rec;
            }
        } else if (mOptions == Options::MAXIMIZE) {
            if (rec > mRec) {
                rv = true;
                mRec = rec;
            }
        } else if (mOptions == Options::NO_CHANGE) {
            rv = false;
        } else if (mOptions == Options::ALWAYS_CHANGE) {
            rv = true;
            mRec = rec;
        }
        if (rv) {
            VecUtils::vecCopy(mX.size(), x, (FT*)mX);
        }
        return rv;
    }

    Options& getOptions() {
        return mOptions;
    }

private:

    unsigned int mOptions;
    FT mRec;
    SmartArrayPtr<FT> mX;

};

#endif	/* UNCONSRECSTORE_HPP */

