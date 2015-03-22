/* 
 * File:   seqsched2.hpp
 * Author: posypkin
 *
 * Sequential scheduler with 2 hueristics: one to run before search another
 * one to run during the search
 * Created on October 15, 2012, 10:05 AM
 */

#ifndef SEQSCHED2_HPP
#define	SEQSCHED2_HPP

#include <limits>
#include <vector>
#include <util/common/bnberrcheck.hpp>
#include "bnbscheduler.hpp"
#include "sstrat.hpp"

class SeqSched2 : public BNBScheduler {
public:

    /**
     * Scheduler parameters
     */
    struct Params {

        /**
         * The constuctor
         */
        Params() :
        mIH(1), mIS(1), mMH(0), mMS(SearchStrategies::DFS), mMB(1) {
        }

        /**
         * Number of the initial heuristic to run before search
         * default 1
         */
        int mIH;
        /**
         * Number of steps os the initial heuristic (if applicable)
         * default 1
         */
        int mIS;
        /**
         * The second (main heuristic to run during the search)
         * default 0
         */
        int mMH;
        /**
         * The number of main tree traverse strategy
         * default DFS
         */
        int mMS;
        /**
         * The number of the default bounding method
         * default 1
         */
        int mMB;

    };

    /**
     * Scheduler states
     */
    struct States {
        /**
         * Terminal state
         */
        static const int T = 0;
        /**
         * Initial state
         */
        static const int I = 1;
        /**
         * Setting initial search strategy
         */
        static const int SETTINGSTRAT_I = 2;
        /**
         * Setting initial heuristic
         */
        static const int SETTINGHEUR_I = 3;
        /**
         * Running heuristic before solving
         */
        static const int SOLVING_I = 4;
        /**
         * Setting main heuristic
         */
        static const int SETTINGHEUR_M = 5;
        /**
         * Setting bounding method
         */
        static const int SETTING_BOUNDING_METHOD = 6;
        /**
         * Setting main search stategy
         */
        static const int SETTINGSTRAT_M = 7;
        /**
         * Main resolution process
         */
        static const int SOLVING_M = 8;
    };

    /**
     * The constructor
     */
    SeqSched2() : mState(States::I) {

    }

    /**
     * Retrieve reference to parameters
     * @return reference to parameters
     */
    Params& getParams() {
        return mParams;
    }

    /**
     * Take an action upon an event
     * @param event incoming event
     * @param info information about the solver state
     * @param action action to take
     */
    void action(Event& event, const SolverInfo& info, Action& action) {
        while (true) {
            switch (mState) {
                case States::I:
                    BNB_ASSERT(event.mCode == Events::START);
                    mState = States::SETTINGSTRAT_I;
                    action.mCode = BNBScheduler::Actions::SET_SEARCH_STRATEGY;
                    action.mArgs[0] = SearchStrategies::NOS;
                    break;
                case States::SETTINGSTRAT_I:
                    BNB_ASSERT(event.mCode == Events::SEARCH_STRATEGY_SET);
                    mState = States::SETTINGHEUR_I;
                    action.mCode = BNBScheduler::Actions::SET_HEURISTIC;
                    action.mArgs[0] = mParams.mIH;
                    break;
                case States::SETTINGHEUR_I:
                    BNB_ASSERT(event.mCode == Events::HEURISTIC_SET);
                    mState = States::SOLVING_I;
                    action.mCode = BNBScheduler::Actions::SOLVE;
                    action.mArgs[0] = mParams.mIS;
                    break;
                case States::SOLVING_I:
                    BNB_ASSERT(event.mCode == Events::DONE);
                    mState = States::SETTINGHEUR_M;
                    action.mCode = BNBScheduler::Actions::SET_HEURISTIC;
                    action.mArgs[0] = mParams.mMH;
                    break;
                case States::SETTINGHEUR_M:
                    BNB_ASSERT(event.mCode == Events::HEURISTIC_SET);
                    mState = States::SETTING_BOUNDING_METHOD;
                    action.mCode = BNBScheduler::Actions::SET_BOUNDING_METHOD;
                    action.mArgs[0] = mParams.mMB;
                    break;
                case States::SETTING_BOUNDING_METHOD:
                    BNB_ASSERT(event.mCode == Events::BOUNDING_METHOD_SET);
                    mState = States::SETTINGSTRAT_M;
                    action.mCode = BNBScheduler::Actions::SET_SEARCH_STRATEGY;
                    action.mArgs[0] = mParams.mMS;
                    break;
                case States::SETTINGSTRAT_M:
                    BNB_ASSERT(event.mCode == Events::SEARCH_STRATEGY_SET);
                    mState = States::SOLVING_M;
                    action.mCode = BNBScheduler::Actions::SOLVE;
                    action.mArgs[0] = std::numeric_limits<long long>::max();
                    break;
                case States::SOLVING_M:
                    BNB_ASSERT(event.mCode == Events::DONE);
                    mState = States::T;
                    action.mCode = BNBScheduler::Actions::EXIT;
                    break;
                default:
                    BNB_ERROR_REPORT("Illegal state\n");
            }
            break;
        }
    }

    Params mParams;

    int mState;

};

#endif	/* SEQSCHED2_HPP */

