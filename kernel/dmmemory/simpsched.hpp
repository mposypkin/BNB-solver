/* 
 * File:   simpsched.hpp
 * Author: posypkin
 *
 * Simple scheduler
 *
 * Created on September 15, 2011, 4:15 AM
 */

#ifndef SIMPSCHED_HPP
#define	SIMPSCHED_HPP

#include <limits>
#include <vector>
#include <util/common/bnberrcheck.hpp>
#include "bnbscheduler.hpp"
#include "sstrat.hpp"

class SimpSched : public BNBScheduler {
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
    };

    struct MasterStates {
        /**
         * Before solving
         */
        static const int PRESOLVING = 3;
        /**
         * Solving the problem
         */
        static const int SOLVING = 4;
        /**
         * Checking conditions
         */
        static const int CHECKING = 5;
        /**
         * Sending data state 1
         */
        static const int SENDING1 = 6;
        /**
         * Sending data state 2
         */
        static const int SENDING2 = 7;
        /**
         * Receiving state
         */
        static const int RECEIVING = 8;
        /**
         * Before end state
         */
        static const int BEFORE_END = 9;
    };

    struct SlaveStates {
        /**
         * The state before the solving
         */
        static const int PRESOLVING = 3;
        /**
         * Receiving data or command state
         */
        static const int RECEIVING = 4;
        /**
         * Solving state
         */
        static const int SOLVING = 5;
        /**
         * Sending state
         */
        static const int SENDING = 6;
    };

    /**
     * Commands for message passing between processes
     */
    struct Commands {
        static const int FINISH = 1;
    };

    /**
     * Constructor
     * @param steps number of steps to perform on the master process
     */
    SimpSched(int steps) {
        mSteps = steps;
        mState = States::I;
        mMasterSearchStrategy = SearchStrategies::WFS;
        mSlaveSearchStrategy = SearchStrategies::DFS;
    }

    void setSearchStrategies(int master, int slave) {
        mMasterSearchStrategy = master;
        mSlaveSearchStrategy = slave;
    }

    void action(const Event& event, const SolverInfo& info, Action& action) {
        if (mRank == 0)
            actionMaster(event, info, action);
        else
            actionSlave(event, info, action);
    }


private:

    void actionMaster(const Event& event, const SolverInfo& info, Action& action) {
        while (true) {
            switch (mState) {
                case States::I:
                    mState = MasterStates::PRESOLVING;
                    BNB_ASSERT(event.mCode == Events::START);
                    action.mCode = Actions::SET_SEARCH_STRATEGY;
                    action.mArgs[0] = mMasterSearchStrategy;
                    break;
                case MasterStates::PRESOLVING:
                    BNB_ASSERT(event.mCode == Events::SEARCH_STRATEGY_SET);
                    action.mCode = Actions::SOLVE;
                    action.mArgs[0] = mSteps;
                    mState = MasterStates::SOLVING;
                    break;
                case MasterStates::SOLVING:
                    BNB_ASSERT(event.mCode == Events::DONE);
                    mState = MasterStates::CHECKING;
                    for (int i = 1; i < mSize; i++) {
                        mFreeProcs.push_back(i);
                    }
                    continue;
                case MasterStates::CHECKING:
                    if (mSize == 1) {
                        mState = States::T;
                        action.mCode = BNBScheduler::Actions::EXIT;
                    } else {
                        if (info.mNSub > 0) {
                            if (mFreeProcs.size() > 0) {
                                mState = MasterStates::SENDING1;
                                action.mCode = BNBScheduler::Actions::SEND_SUB_AND_RECORDS;
                                action.mArgs[0] = mFreeProcs.back();
                                action.mArgs[1] = 1;
                                mFreeProcs.pop_back();
                            } else {
                                mState = MasterStates::RECEIVING;
                                action.mCode = BNBScheduler::Actions::RECV;
                                action.mArgs[0] = -1;
                            }
                        } else {
                            if (mFreeProcs.size() < mSize - 1) {
                                mState = MasterStates::RECEIVING;
                                action.mCode = BNBScheduler::Actions::RECV;
                                action.mArgs[0] = -1;
                            } else {
                                mState = MasterStates::BEFORE_END;
                                continue;
                            }
                        }
                    }
                    break;
                case MasterStates::SENDING1:
                    BNB_ASSERT(event.mCode == Events::SENT);
                    mState = MasterStates::CHECKING;
                    continue;
                case MasterStates::RECEIVING:
                    BNB_ASSERT(event.mCode == Events::DATA_ARRIVED);
                    mState = MasterStates::CHECKING;
                    mFreeProcs.push_back(event.mArgs[0]);
                    continue;
                case MasterStates::BEFORE_END:
                    if (mFreeProcs.size() > 0) {
                        mState = MasterStates::SENDING2;
                        action.mCode = BNBScheduler::Actions::SEND_COMMAND;
                        action.mArgs[0] = mFreeProcs.back();
                        action.mArgs[1] = Commands::FINISH;
                        mFreeProcs.pop_back();
                    } else {
                        mState = States::T;
                        action.mCode = BNBScheduler::Actions::EXIT;
                    }
                    break;
                case MasterStates::SENDING2:
                    BNB_ASSERT(event.mCode == Events::SENT);
                    mState = MasterStates::BEFORE_END;
                    continue;
            }
            break;
        }
    }

    void actionSlave(const Event& event, const SolverInfo& info, Action& action) {
        switch (mState) {
            case States::I:
                mState = MasterStates::PRESOLVING;
                BNB_ASSERT(event.mCode == Events::START);
                action.mCode = Actions::SET_SEARCH_STRATEGY;
                action.mArgs[0] = mSlaveSearchStrategy;
                break;
            case SlaveStates::PRESOLVING:
                BNB_ASSERT(event.mCode == Events::SEARCH_STRATEGY_SET);
                action.mCode = BNBScheduler::Actions::RECV;
                action.mArgs[0] = 0;
                mState = SlaveStates::RECEIVING;
                break;
            case SlaveStates::RECEIVING:
                BNB_ASSERT((event.mCode == Events::COMMAND_ARRIVED) || (event.mCode == Events::DATA_ARRIVED));
                if (event.mCode == Events::DATA_ARRIVED) {
                    mState = SlaveStates::SOLVING;
                    action.mCode = BNBScheduler::Actions::SOLVE;
                    action.mArgs[0] = std::numeric_limits<long long>::max();
                } else {
                    BNB_ASSERT(event.mArgs[1] == Commands::FINISH);
                    mState = States::T;
                    action.mCode = BNBScheduler::Actions::EXIT;
                }
                break;
            case SlaveStates::SOLVING:
                BNB_ASSERT(event.mCode == Events::DONE);
                mState = SlaveStates::SENDING;
                action.mCode = BNBScheduler::Actions::SEND_RECORDS;
                action.mArgs[0] = 0;
                break;
            case SlaveStates::SENDING:
                BNB_ASSERT(event.mCode == Events::SENT);
                action.mCode = BNBScheduler::Actions::RECV;
                action.mArgs[0] = 0;
                mState = SlaveStates::RECEIVING;
                break;
        }

    }


    int mSteps;
    int mState;
    int mMasterSearchStrategy;
    int mSlaveSearchStrategy;
    std::vector<int> mFreeProcs;
};



#endif	/* SIMPSCHED_HPP */

