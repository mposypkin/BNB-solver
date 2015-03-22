/* 
 * File:   multicov.hpp
 * Author: posypkin
 *
 * Created on April 27, 2011, 6:25 PM
 */

#ifndef MULTICOV_HPP
#define	MULTICOV_HPP

#include <util/box/box.hpp>
#include <util/box/boxutils.hpp>
#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/pareto/multiopt/multipoint.hpp>
#include <problems/pareto/multiopt/multilocsearch.hpp>
#include <util/boxbnb/boxsub.hpp>
#include <util/boxbnb/boxsubpool.hpp>
#include "bounder.hpp"
#include "lipbounder.hpp"


#define LT 50

#define UT 100

/**
 * Coverage method for multiobjective optimization
 */
template <typename FT> class MultiCov {
public:

    /**
     * Constructor
     * @param pd problem data
     */
    MultiCov(MultiData<FT>& pd) : mMD(pd), mEps(0.1), mDelt(0.1), mCurBounder(1), mCurHeur(0) {
    }

    /**
     * Performs several algorithm steps
     * @param state state
     * @param niters maximal number of iterations to perform, on return - the actual number of iterations performed
     * @param improved (output) true if the record was updated and false otherwise
     */
    template <class State> void solve(State& state, long long int& niters, bool* improved = NULL) {
        int pd = mMD.mBox.mDim;
        int crid = mMD.mCriteria.size();
        int consd = mMD.mConstraints.size();
        Point<FT> cribound(crid);
        Point<FT> consbound(consd);
        int i;

        if (improved)
            *improved = false;
        for (i = 1; i <= niters; i++) {
            int sz = state.mSubBag.size();
#if 0
            printf("<%d>\n", sz);
            if (sz < LT) {
                state.mSubBag.setDirection(SubBag<double, WidFirst<double> >::Directions::TAKE_LARGEST);
            } else if (sz > UT) {
                state.mSubBag.setDirection(SubBag<double, WidFirst<double> >::Directions::TAKE_SMALLEST);
            }
#endif

            if (state.mSubBag.empty()) {
                break;
            } else {
                BoxSub<FT> ms(pd);
                ms = state.mSubBag.get();
                MultiPoint<FT> mp(pd, crid);
                Point<FT> cp(consd);
                BoxUtils::getCenter(ms.mBox, (double*) mp.mParam.mX);
                mMD.evaluate(mp.mParam.mX, mp.mCrit.mX, cp.mX);
                mBounderVector[mCurBounder - 1]->getLowerBound(ms.mBox, cribound.mX, consbound.mX);
                for (int j = 0; j < crid; j++)
                    cribound.mX[j] += mEps;
                improve(mp);
                if (check(mp.mParam.mX)) {
                    if (state.mPointBag.put(mp)) {
                        if (improved)
                            *improved = true;
                    }
                }
                bool infeasible = false;
                for (int j = 0; j < consd; j++) {
                    if (consbound.mX[j] > 0) {
                        infeasible = true;
                        break;
                    }
                }
                if (infeasible) {
                    continue;
                } else if (state.mPointBag.dominate(cribound)) {
                    // printf("dom\n");
                    continue;
                } else {
                    divide(ms, state);
                    // printf("devide\n");
                }
            }
        }
        niters = i;
    }

    /**
     * Setup precision
     * @param eps new epsilon for criteria
     * @param delt new delt for constraints
     */
    void setPrecision(FT eps, FT delt) {
        mEps = eps;
        mDelt = delt;
    }

    /**
     * Add bounder to the end of the bounders list
     * @param bd bounder
     */
    void addBounder(Bounder<FT>* bd) {
        mBounderVector.push_back(bd);
    }

    /**
     * Adds local search method to the end of the local search methods list
     * @param ls local search method
     */
    void addHeuristic(MultiLocalSearch<double>* ls) {
        mHeurVector.push_back(ls);
    }

    /**
     * Retrieve problem source data
     * @return problem data
     */
    MultiData<FT>& getMultiData() {
        return mMD;
    }

    void setHeuristic(int heuristic) {
        BNB_ASSERT(heuristic <= mHeurVector.size());
        mCurHeur = heuristic;
    }

    /**
     * Setup bounding method
     * the bigger number the more complex and powerful bounding method
     * @param method bounding method to set
     */
    void setBoundingMethod(int method) {
        BNB_ASSERT(method <= mBounderVector.size());
        mCurBounder = method;
    }

private:

    bool check(const FT* x) const {
        bool rv = true;
        int n = mMD.mConstraints.size();
        for (int i = 0; i < n; i++) {
            if (mMD.mConstraints[i]->func(x) > mDelt) {
                rv = false;
                break;
            }
        }
        return rv;
    }

    void improve(MultiPoint<FT>& mp) {
        if (mCurHeur > 0) {
            mHeurVector[mCurHeur - 1]->search(mp);
        }
    }

    template <class State> void divide(const BoxSub<FT>& ms, State& state) {
        int n = mMD.mBox.mDim;
        int d = ms.mDepth;
        //printf("divide: %s\n", state.toString().c_str());
        BoxSub<FT> msl(n, d + 1);
        BoxSub<FT> msr(n, d + 1);
        BoxUtils::divideByLongestEdge(ms.mBox, msl.mBox, msr.mBox);
        state.mSubBag.put(msl);
        state.mSubBag.put(msr);
    }

    MultiData<FT> mMD;
    FT mEps;
    FT mDelt;
    Bounder<FT>* mBounder;
    std::vector< MultiLocalSearch<double>* > mHeurVector;
    std::vector< Bounder<FT>* > mBounderVector;
    int mCurBounder;
    int mCurHeur;
};


#endif	/* MULTICOV_HPP */

