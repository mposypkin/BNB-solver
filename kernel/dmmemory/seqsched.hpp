/* 
 * File:   seqsched.hpp
 * Author: posypkin
 *
 * Simple dequential scheduler with dynamic tuning of the search strategy
 * possibility of adjusting the bounding method and heuristic
 *
 * Created on April 14, 2012, 4:15 AM
 */

#ifndef SEQSCHED_HPP
#define	SEQSCHED_HPP

#include <limits>
#include <vector>
#include <util/common/bnberrcheck.hpp>
#include "bnbscheduler.hpp"
#include "sstrat.hpp"

class SeqSched : public BNBScheduler {
public:

    /**
     * States
     */
    struct States {
        /**
         * Initial state
         */
        static const int I = 1;
        /**
         * Terminal state
         */
        static const int T = 2;
        /**
         * Solving the problem
         */
        static const int SOLVING = 4;
        /**
         * Checking conditions for changing strategy
         */
        static const int CHECKING_S = 5;
        /**
         * Setting the strategy
         */
        static const int SSTRAT = 6;
        /**
         * Setting the heuristic
         */
        static const int SETTING_HEUR = 7;
        /**
         * Setting the bounding method
         */
        static const int SETTING_B = 8;
    };

    /**
     * Constructor
     */
    SeqSched() {
        mSteps = 1;
        mState = States::I;
        mInitSearchStrategy = SearchStrategies::DFS;
        mL = 0;
        mU = 0;
        mDoTuning = true;
        mIters = 0;
        mInitHeur = 0;
        mInitBoundingMethod = 1;
    }

    /**
     * Set control interval
     * @param steps number of steps to perform between control points
     */
    void setControlInterval(int steps) {
        mSteps = steps;
    }

    /**
     * Setting search strategy parameters
     * @param is initial search strategy
     * @param l lower limit to start WFS
     * @param u uppser limit to starg DFS
     */
    void setSearchStrategyParams(int is, int l, int u) {
        mInitSearchStrategy = is;
        mL = l;
        mU = u;
    }

    /**
     * Set initial bounding method
     * @param bm initial bounding method
     */
    void setBoundingMethod(int bm){
        mInitBoundingMethod = bm;
    }
    /**
     * Set heuristic related parameters
     * @param ih initial heuristic
     */
    void setHeuristic(int ih) {
        mInitHeur = ih;
    }



    /**
     * Enables and disables tuning
     * @param doTuning enables tuning if true and disables tuning if false
     */
    void enableTuning(bool doTuning) {
        mDoTuning = doTuning;
    }

    void action(const Event& event, const SolverInfo& info, Action& action) {
        while (true) {
            switch (mState) {
                case States::I:
                    BNB_ASSERT(event.mCode == Events::START);
                    mIters = 0;
                    mState = States::SETTING_B;
                    action.mCode = BNBScheduler::Actions::SET_BOUNDING_METHOD;
                    action.mArgs[0] = mInitBoundingMethod;
                    break;
                case States::SOLVING:
                    BNB_ASSERT(event.mCode == Events::DONE);
                    mState = States::CHECKING_S;
                    continue;
                case States::CHECKING_S:
                    BNB_ASSERT(event.mCode == Events::DONE);
                    mIters += event.mArgs[0];
                    if (info.mNSub == 0) {
                        mState = States::T;
                        action.mCode = BNBScheduler::Actions::EXIT;
                    } else if (mDoTuning && (info.mNSub > mU)) {
                        mState = States::SSTRAT;
                        action.mCode = BNBScheduler::Actions::SET_SEARCH_STRATEGY;
                        action.mArgs[0] = SearchStrategies::DFS;
                    } else if (mDoTuning && (info.mNSub < mL)) {
                        mState = States::SSTRAT;
                        action.mCode = BNBScheduler::Actions::SET_SEARCH_STRATEGY;
                        action.mArgs[0] = SearchStrategies::WFS;
                    } else {
                        mState = States::SOLVING;
                        action.mCode = BNBScheduler::Actions::SOLVE;
                        action.mArgs[0] = mSteps;
                    }
                    break;
                case States::SSTRAT:
                    BNB_ASSERT(event.mCode == Events::SEARCH_STRATEGY_SET);
                    mState = States::SOLVING;
                    action.mCode = Actions::SOLVE;
                    action.mArgs[0] = mSteps;
                    break;
                case States::SETTING_B:
                    BNB_ASSERT(event.mCode == Events::BOUNDING_METHOD_SET);
                    mState = States::SETTING_HEUR;
                    action.mCode = Actions::SET_HEURISTIC;
                    action.mArgs[0] = mInitHeur;
                    break;
                case States::SETTING_HEUR:
                    BNB_ASSERT(event.mCode == Events::HEURISTIC_SET);
                    mState = States::SSTRAT;
                    action.mCode = BNBScheduler::Actions::SET_SEARCH_STRATEGY;
                    action.mArgs[0] = mInitSearchStrategy;
                    break;
                default:
                    BNB_ERROR_REPORT("Illegal state\n");
            }
            break;
        }

    }


    int mSteps;
    int mState;
    int mU;
    int mL;
    int mInitSearchStrategy;
    int mInitHeur;
    int mInitBoundingMethod;
    long long int mIters;
    bool mDoTuning;
};



#endif	/* SIMPSCHED_HPP */

