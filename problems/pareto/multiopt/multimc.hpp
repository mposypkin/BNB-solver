/* 
 * File:   multimc.hpp
 * Author: posypkin
 *
 * Monte-Carlo method for multiobjective optimization
 *
 * Created on April 19, 2011, 10:53 PM
 */

#ifndef MULTIMC_HPP
#define	MULTIMC_HPP

#include <stdlib.h>
#include <math.h>

#include <util/common/util.hpp>

#include "multidata.hpp"
#include "multipoint.hpp"
#include "mpsimpbag.hpp"
#include "multilocsearch.hpp"

/**
 * Simple Monte-Carlo solver for multiobjective optimization
 */
template <typename FT, class Bag = MPSimpBag<FT> > class MultiMC {
public:

    struct Options {
        /**
         * Use local search to facilitate the search
         */
        static const unsigned int USE_LOCAL_SEARCH = 1;
    };

    struct Counters {
        /**
         * Number of iterations
         */
        int mIters;

        /**
         * BagSize
         */
        int mBagSize;
    };
    /**
     * Constructor
     * @param pd problem data
     * @param sd solver data
     */
    MultiMC(MultiData<FT>& pd) : mMD(pd), mLS(NULL), mOptions(0){
        mMaxIters = getMaxValue<int>();
        mMaxBagSize = getMaxValue<int>();
    }

    /**
     * Setup maximal number of iterations
     * @param nit
     */
    void setMaxIters(int nit) {
        mMaxIters = nit;
    }

    /**
     * Setup maximal bag size
     * @param mbs maximal bag size
     */
    void setMaxBagSize(int mbs){
        mMaxBagSize = mbs;
    }

    /**
     * Performs the solution process until given number of iterations
     * or the size of the bag are exceeded
     */
    void solve() {
        mCounters.mIters = 0;
        for (;;mCounters.mIters ++) {
            mCounters.mBagSize = BNBMAX(mCounters.mBagSize, mBag.size());
            if((mCounters.mIters >= mMaxIters)||(mCounters.mBagSize >= mMaxBagSize))
                break;
            MultiPoint<FT> mp(2, 2);
            generate(mp.mParam);
            if (check(mp.mParam)) {
                mMD.evaluate(mp.mParam.mX, mp.mCrit.mX, NULL);
                improve(mp);
                mBag.put(mp);
            }
        }
    }

    Bag& getResult() {
        return mBag;
    }

    /**
     * Setup Local Search utility
     *
     * @param ls the pointer to the local search utility
     */
    void setLocalSearch(MultiLocalSearch<FT>* ls) {
        mLS = ls;
    }

    /**
     * Returns options
     * @return options variable
     */
    unsigned int& getOptions() {
        return mOptions;
    }

    /**
     * Retrieve counters
     * @return counters
     */
    const Counters& getCounters() const {
        return mCounters;
    }
private:

    void generate(Point<FT>& p) {
        for (int i = 0; i < p.mDim; i++) {
            FT a = mMD.mBox.mA[i];
            FT b = mMD.mBox.mB[i];
            FT r = (FT) rand() / (FT) RAND_MAX;

            FT x = (b - a) * r + a;
            p.mX[i] = x;
        }
    }

    bool check(Point<FT>& p) {
        bool rv = true;
        typename std::vector< Objective<FT>* >::const_iterator i = mMD.mConstraints.begin();
        for (; i != mMD.mConstraints.end(); i++) {
            if ((*i)->func(p.mX) > 0) {
                rv = false;
                break;
            }
        }
        return rv;
    }
/*
    void evaluate(MultiPoint<FT>& mp) {
        typename std::vector< Objective<FT>* >::const_iterator i = mMD.mCriteria.begin();
        int j = 0;
        for (; i != mMD.mCriteria.end(); i++) {
            mp.mCrit.mX[j++] = (*i)->func(mp.mParam.mX);
        }
    }
*/
    void improve(MultiPoint<FT>& mp) {
        if (mOptions & Options::USE_LOCAL_SEARCH) {
            BNB_ASSERT(mLS);
            mLS->search(mp);
        }
    }

    MultiData<FT> mMD;
    Bag mBag;
    unsigned int mOptions;
    int mMaxIters;
    int mMaxBagSize;
    MultiLocalSearch<FT> *mLS;
    Counters mCounters;
};


#endif	/* MULTIMC_HPP */

