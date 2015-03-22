#ifndef BASESSUM_HPP
#define	BASESSUM_HPP
/* 
 * File:   basessum.hpp
 * Author: posypkin
 *
 * Basic subset-sum solver
 *
 * Created on April 12, 2012, 10:31 AM
 */

#include <kernel/dmmemory/bnbresolver.hpp>
#include <problems/ssum/comssum/ssum.hpp>
#include <util/common/utilmacro.hpp>
#include <set>
#include "ssumsub.hpp"
#include "ssumsol.hpp"
#include "ssumsubser.hpp"
#include "ssumsolser.hpp"

template <class VT> class BaseSSum : public BNBResolver {
public:

    /**
     * Comparison of 2 subproblems (needed for the subproblems container)
     */
    class Cmp {
    public:

        bool operator() (const SSumSub<VT>& s1, const SSumSub<VT>& s2) const {
            return (s1.mFixedItems < s2.mFixedItems);
        }
    };


    /**
     * The type of the container
     */
    typedef std::multiset< SSumSub<VT>, Cmp > QType;

    /**
     * The constructor
     * @param ssum the subset sum problem definition
     */
    BaseSSum(SSum<VT>* problem) : mRecord(problem->mDim) {
        mProblem = problem;
        mStrategy = SearchStrategies::DFS;
    }

    /**
     * Get solver state info
     * @param info information structure to fill
     */
    void getInfo(SolverInfo& info) {
        info.mNSub = mSubs.size();
        info.mRecUpdated = mUpdated;
    }

    /**
     * Reset solver state info
     */
    void resetInfo() {
        mUpdated = false;
    }

    /**
     * Push subsets to the serializer
     *
     * @param num number of subsets
     * @param ser the serializer
     */
    void getSubs(int& num, BinarySerializer& ser) {
        int i;
        int n = BNBMIN(num, mSubs.size());
        ser << n;
        for (i = 0; i < n; i++) {
            SSumSub<VT> sp = get();
            ser << sp;
        }
        num = n;
    }

    /**
     * Push record(s) to the serializer
     * @param set the serilizer
     */
    void getRecords(BinarySerializer& ser) {
        ser << mRecord;
    }

    /**
     * Take subproblems from the serializer and put them to the solver
     *
     * @param ser serializer
     * @return number of packed problems
     */
    int putSubs(BinarySerializer& ser) {
        int n;
        ser >> n;
        for (int i = 0; i < n; i++) {
            SSumSub<VT> sp(mProblem->mDim);
            ser >> sp;
            mSubs.insert(sp);
        }
        return n;
    }

    /**
     * Take record(s) from the serializer and put them to the solver
     *
     * @param ser serializer
     */
    void putRecords(BinarySerializer& ser) {
        ser >> mRecord;
    }

    /**
     * Perform several algorithmic steps (or less)
     * @param steps on entry: requested number of steps, on exit: actual number of steps
     *        -1 on exit means an error
     */
    void solve(long long& steps) {
        int i = 0;
        while (true) {
            if (i >= steps)
                break;
            if (mSubs.empty())
                break;
            SSumSub<VT> s = get();
            int n = mProblem->mDim;
            int k = s.mFixedItems;
            SSumSub<VT> s1(n);
            s1.mRemCapacity = s.mRemCapacity - mProblem->mWeights[k];
            if (s1.mRemCapacity == 0) {
                mRecord.mValue = mProblem->mCapacity;
                for (int i = 0; i < k; i++) {
                    mRecord.mValues[i] = s.mValues[i];
                }
                mRecord.mValues[k] = 1;
                for (int i = k + 1; i < n; i++) {
                    mRecord.mValues[i] = 0;
                }
            } else if (s1.mRemCapacity > 0) {
                for (int i = 0; i < n; i++) {
                    s1.mValues[i] = s.mValues[i];
                }
                s1.mValues[k] = 1;
                s1.mFixedItems = k + 1;
                s1.mRemWeight = s.mRemWeight - mProblem->mWeights[k];
                mSubs.insert(s1);
            }
            SSumSub<VT> s0(n);
            s0.mRemCapacity = s.mRemCapacity;
            s0.mRemWeight = s.mRemWeight - mProblem->mWeights[k];
            if (s0.mRemWeight <= s0.mRemCapacity) {
                VT nv = mProblem->mCapacity - s0.mRemCapacity + s0.mRemWeight;
                if (nv > mRecord.mValue) {
                    for (int i = 0; i < k; i++) {
                        mRecord.mValues[i] = s.mValues[i];
                    }
                    mRecord.mValues[k] = 0;
                    for (int i = k + 1; i < n; i++) {
                        mRecord.mValues[i] = 1;
                    }
                    mRecord.mValue = nv;
                }
            } else {
                for (int i = 0; i < n; i++) {
                    s0.mValues[i] = s.mValues[i];
                }
                s0.mValues[k] = 0;
                s0.mFixedItems = k + 1;
                mSubs.insert(s0);
            }
            i++;
        }
        steps = i;
    }

    /**
     * Setup search strategy
     * @param strategy the search strategy
     */
    void setSearchStrategy(int strategy) {
        mStrategy = strategy;
    }

    /**
     * Setup the search process by generating the root of the B&B tree
     */
    void generateRoot() {
        SSumSub<VT> sp(mProblem->mDim);
        sp.mRemCapacity = mProblem->mCapacity;
        sp.mFixedItems = 0;
        sp.mRemWeight = 0;
        for (int i = 0; i < mProblem->mDim; i++) {
            sp.mValues[i] = -1;
            sp.mRemWeight += mProblem->mWeights[i];
        }
        mSubs.insert(sp);
    }

    /**
     * Retrieve the best known (record) solution
     * @return record solution
     */
    SSumSolution<VT>& getRecord() {
        return mRecord;
    }

    void setBoundingMethod(int method) {
    }

    void setHeuristic(int heuristic) {
    }

private:

    SSumSub<VT> get() {
        typename QType::iterator i;
        if (mStrategy == SearchStrategies::DFS) {
            i = mSubs.end();
            i--;
        } else {
            i = mSubs.begin();
        }
        SSumSub<VT> b = *i;
        mSubs.erase(i);
        return b;
    }

    SSum<VT>* mProblem;
    QType mSubs;
    bool mUpdated;
    int mStrategy;
    SSumSolution<VT> mRecord;
};


#endif	/* BASESSUM_HPP */

