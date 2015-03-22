/* 
 * File:   seqsched3.hpp
 * Author: posypkin
 * Sequential scheduler 3
 * Created on December 2, 2012, 5:51 PM
 */

#ifndef SEQSCHED3_HPP
#define	SEQSCHED3_HPP
#include <limits>
#include <vector>
#include <util/common/bnberrcheck.hpp>
#include "bnbscheduler.hpp"
#include "sstrat.hpp"

class SeqSched3 : public BNBScheduler {
public:

    /**
     * Scheduler parameters
     */
    struct Params {

        /**
         * The constuctor
         */
        Params() :
        mIH(1), mIHS(1), mMH(0), mMHS(1), mMBR(SearchStrategies::DFS), mMBRS(1), mMB(1) {
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
        int mIHS;
        /**
         * The secondary (main heuristic to run during the search)
         * default 0
         */
        int mMH;
        /**
         * Number of steps of the secondary heuristic (default 1)
         */
        int mMHS;
        /**
         * The number of main tree traverse strategy
         * default DFS
         */
        int mMBR;
        /**
         * The number of steps of the main traverse strategy before the break
         */
        int mMBRS;
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
         * Setting initial search strategy to zero
         */
        static const int ZERO_STRAT_I = 2;
        /**
         * Setting bounding method
         */
        static const int SETTING_BOUNDER = 3;
        /**
         * Setting initial heuristic
         */
        static const int SETTINGHEUR_I = 4;
        /**
         * Running heuristic before solving
         */
        static const int SOLVING_I = 5;
        /**
         * Checking the termination criterion
         */
        static const int CHECKING = 6;
        /**
         * Setting main heuristic
         */
        static const int SETTINGHEUR_M = 7;

        /**
         * Setting main search stategy to zero
         */
        static const int ZERO_STRAT_M = 8;
        /**
         * Run heuristics in the main loop
         */
        static const int SOLVING_M_H = 9;
        /**
         * Set main heuristic to zero
         */
        static const int ZERO_HEUR_M = 10;
        /**
         * Setting main strategy
         */
        static const int SETTINGSTRAT_M = 11;
        /**
         * Branching in the main loop
         */
        static const int SOLVING_M_B = 12;
    };

    /**
     * The constructor
     */
    SeqSched3() : mState(States::I) {

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
                    mState = States::ZERO_STRAT_I;
                    action.mCode = BNBScheduler::Actions::SET_SEARCH_STRATEGY;
                    action.mArgs[0] = SearchStrategies::NOS;
                    break;
                case States::ZERO_STRAT_I:
                    BNB_ASSERT(event.mCode == Events::SEARCH_STRATEGY_SET);
                    mState = States::SETTING_BOUNDER;
                    action.mCode = BNBScheduler::Actions::SET_BOUNDING_METHOD;
                    action.mArgs[0] = mParams.mMB;
                    break;
                case States::SETTING_BOUNDER:
                    BNB_ASSERT(event.mCode == Events::BOUNDING_METHOD_SET);
                    mState = States::SETTINGHEUR_I;
                    action.mCode = BNBScheduler::Actions::SET_HEURISTIC;
                    action.mArgs[0] = mParams.mIH;
                    break;
                case States::SETTINGHEUR_I:
                    BNB_ASSERT(event.mCode == Events::HEURISTIC_SET);
                    mState = States::SOLVING_I;
                    action.mCode = BNBScheduler::Actions::SOLVE;
                    action.mArgs[0] = mParams.mIHS;
                    break;
                case States::SOLVING_I:
                    BNB_ASSERT(event.mCode == Events::DONE);
                    mState = States::CHECKING;
                    continue;
                case States::CHECKING:
                    if (info.mNSub == 0) {
                        mState = States::T;
                        action.mCode = BNBScheduler::Actions::EXIT;
                    } else {
                        mState = States::SETTINGHEUR_M;
                        action.mCode = BNBScheduler::Actions::SET_HEURISTIC;
                        action.mArgs[0] = mParams.mMH;
                    }
                    break;
                case States::SETTINGHEUR_M:
                    BNB_ASSERT(event.mCode == Events::HEURISTIC_SET);
                    mState = States::ZERO_STRAT_M;
                    action.mCode = BNBScheduler::Actions::SET_SEARCH_STRATEGY;
                    action.mArgs[0] = SearchStrategies::NOS;
                    break;
                case States::ZERO_STRAT_M:
                    BNB_ASSERT(event.mCode == Events::SEARCH_STRATEGY_SET);
                    mState = States::SOLVING_M_H;
                    action.mCode = BNBScheduler::Actions::SOLVE;
                    action.mArgs[0] = mParams.mMHS;
                    break;
                case States::SOLVING_M_H:
                    BNB_ASSERT(event.mCode == Events::DONE);
                    mState = States::ZERO_HEUR_M;
                    action.mCode = BNBScheduler::Actions::SET_HEURISTIC;
                    action.mArgs[0] = 0;
                    break;
                case States::ZERO_HEUR_M:
                    BNB_ASSERT(event.mCode == Events::HEURISTIC_SET);
                    mState = States::SETTINGSTRAT_M;
                    action.mCode = BNBScheduler::Actions::SET_SEARCH_STRATEGY;
                    action.mArgs[0] = mParams.mMBR;
                    break;
                 case States::SETTINGSTRAT_M:
                    BNB_ASSERT(event.mCode == Events::SEARCH_STRATEGY_SET);
                    mState = States::SOLVING_M_B;
                    action.mCode = BNBScheduler::Actions::SOLVE;
                    action.mArgs[0] = mParams.mMBRS;
                    break;
                 case States::SOLVING_M_B:
                    BNB_ASSERT(event.mCode == Events::DONE);
                    mState = States::CHECKING;
                    continue;
                default:
                    BNB_ERROR_REPORT("Illegal state\n");
            }
            break;
        }

    }

private:
    Params mParams;
    int mState;

};

#endif	/* SEQSCHED3_HPP */

