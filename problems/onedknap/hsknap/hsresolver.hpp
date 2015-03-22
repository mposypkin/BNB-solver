/* 
 * File:   hsresolver.hpp
 * Author: posypkin
 *
 * Created on August 28, 2012, 10:39 AM
 */

#ifndef HSRESOLVER_HPP
#define	HSRESOLVER_HPP

#include <kernel/dmmemory/bnbresolver.hpp>
#include <kernel/dmmemory/sstrat.hpp>
#include <problems/onedknap/common/knapdata.hpp>
#include "hsbounder.hpp"
#include "hssub.hpp"
#include "hssol.hpp"
#include "hsutils.hpp"
#include "hsbag.hpp"
#include "hsheur.hpp"

/**
 * Main resolver class for Horovitz-Sahni BnB method
 */
template <class T> class HSResolver : public BNBResolver {
public:

    /**
     * The constructor
     * @param kd knapsack data
     */
    HSResolver(const KnapData<T>* kd) :
    mKD(kd),
    mCurrentBounder(0),
    mCurrentHeur(0),
    mRecordUpdated(false) {
        mRecord = HSUtils<T>::emptySolution(*mKD);
    }

    /**
     * Resets the record and the problem list to the originial state 
     */
    void reset() {
        mRecord = HSUtils<T>::emptySolution(*mKD);
        mBag.clear();
    }

    /**
     * Adds bounder to the bounders vector
     * @param bnd bounders vector
     */
    void addBounder(HSBounder<T>* bnd) {
        mBounders.push_back(bnd);
    }

    /**
     * Adds heuristics to the heuristics vector
     * @param heur heuristics
     */
    void addHeur(HSHeur<T>* heur) {
        mHeurs.push_back(heur);
    }

    void setSearchStrategy(int strategy) {
        mStrategy = strategy;
    }

    void setHeuristic(int heur) {
        mCurrentHeur = heur;
    }

    void setBoundingMethod(int method) {
        mCurrentBounder = method - 1;
    }

    /**
     * Get record
     * @return record
     */
    const HSSol<T>& getRecord() const {
        return mRecord;
    }

    /**
     * Get solver state info
     * @param info information structure to fill
     */
    void getInfo(SolverInfo& info) {
        info.mNSub = mBag.size();
        info.mRecUpdated = mRecordUpdated;
    }

    /**
     * Reset solver state info
     */
    void resetInfo() {
        mRecordUpdated = false;
    }

    void solve(long long& steps) {
        T oldr = mRecord.mP;
        if (mStrategy == SearchStrategies::DFS) {
            if (mBag.empty())
                steps = 0;
            else {
                HSSub<T> sub = mBag.get();
                long long r;
                DFSearch(steps, r, sub);
                if (sub.mFrozen != sub.mH) {
                    mBag.put(sub);
                }
                steps = r;
            }
        } else if (mStrategy == SearchStrategies::WFS) {
            long long r;
            WFSearch(steps, r);
            steps = r;
        } else if (mStrategy == SearchStrategies::NOS) {
            heurSearch(mBag.top(), steps);
        } else {
            BNB_ERROR_REPORT("Strategy is not implemented\n");
        }
        if (oldr != mRecord.mP) {
            mRecordUpdated = true;
        } else {
            mRecordUpdated = false;
        }
    }

    void heurSearch(const HSSub<T>& sub, long long& steps) {
        if (mCurrentHeur >= 1) {
            BNB_ASSERT(mCurrentHeur <= mHeurs.size());
            mHeurs[mCurrentHeur - 1]->search(sub, mRecord, steps);
        }
        steps = 0; // Do not account heuristic steps
    }

    void WFSearch(long long maxit, long long& realit) {
        long long i = 0;
        while ((i < maxit) && !mBag.empty()) {
            i++;
            //printf("record: %s\n", HSUtils<T>::HSSol2string(mRecord).c_str());
            //printf("bag = %s\n", mBag.toString().c_str());
            HSSub<T> sub = mBag.get();
            //printf("sub = (%s)\n", HSUtils<T>::HSSub2string(sub).c_str());
            if (sub.mH == (sub.mFrozen + 1)) {
                long long r;
                DFSearch(1, r, sub);
                if (sub.mFrozen != sub.mH) {
                    mBag.put(sub);
                }
            } else {
                HSSub<T> subn;
                subn = sub;
                subn.mX.copy(sub.mX);
                while (subn.mH > subn.mFrozen + 1) {
                    subn.mH--;
                    if (subn.mX[subn.mH] == 1) {
                        subn.mP -= mKD->mCoe[subn.mH].mP;
                        subn.mW -= mKD->mCoe[subn.mH].mW;
                    }
                    subn.mFW += mKD->mCoe[subn.mH].mW;
                    subn.mFP += mKD->mCoe[subn.mH].mP;
                    subn.mX[subn.mH] = 2;
                }
                subn.mX[subn.mH] = 0;
                bool bt = mBounders[mCurrentBounder]->BoundAndReduce(*mKD, subn, mRecord);
                if (!bt) {
                    subn.mFrozen = subn.mH;
                    subn.mH++;
                    mBag.put(subn);
                }
                sub.mFrozen++;
                mBag.put(sub);
            }
        }
        realit = i;
    }

    void DFSearch(long long maxit, long long& realit, HSSub<T>& sub) {
        long long i = 0;
        BNB_ASSERT(mCurrentBounder < mBounders.size());
        while ((i < maxit) && (sub.mH != sub.mFrozen)) {
            //printf("sp: %s\n", HSUtils<T>::HSSub2string(sp).c_str());
            //printf("rec: %s\n", HSUtils<T>::HSSol2string(mRecord).c_str());
#if 0
            if (mCurrentHeur != 0) {
                long long int ns = 1;
                heurSearch(sub, ns);
            }
#endif
            i++;
            if (sub.mX[sub.mH] == 2) {
                sub.mX[sub.mH] = 1;
                sub.mP += mKD->mCoe[sub.mH].mP;
                sub.mW += mKD->mCoe[sub.mH].mW;
                sub.mFW -= mKD->mCoe[sub.mH].mW;
                sub.mFP -= mKD->mCoe[sub.mH].mP;
                bool bt = mBounders[mCurrentBounder]->BoundAndReduce(*mKD, sub, mRecord);
                if (!bt)
                    sub.mH++;
            } else if (sub.mX[sub.mH] == 1) {
                sub.mX[sub.mH] = 0;
                sub.mP -= mKD->mCoe[sub.mH].mP;
                sub.mW -= mKD->mCoe[sub.mH].mW;
                if (sub.mFrozen == (sub.mH - 1))
                    sub.mFrozen = sub.mH;
                bool bt = mBounders[mCurrentBounder]->BoundAndReduce(*mKD, sub, mRecord);
                if (!bt)
                    sub.mH++;
            } else if (sub.mX[sub.mH] == 0) {
                sub.mX[sub.mH] = 2;
                sub.mFW += mKD->mCoe[sub.mH].mW;
                sub.mFP += mKD->mCoe[sub.mH].mP;
                sub.mH--;
            }
        }
        realit = i;
    }

    void fetchSub(HSSub<T>& sub) {
        mBag.put(sub);
    }

    void setRecordValue(T val) {
        mRecord.mP = val;
    }

private:
    const KnapData<T>* mKD;
    HSSol<T> mRecord;
    std::vector< HSBounder<T>* > mBounders;
    std::vector< HSHeur<T>* > mHeurs;
    int mCurrentBounder;
    int mCurrentHeur;
    int mStrategy;
    HSBag<T> mBag;
    bool mRecordUpdated;
};

#endif	/* HSRESOLVER_HPP */

