/* 
 * File:   bnbscheduler.hpp
 * The abstract scheduler interface for branch-and-bound distributed memory implementation
 */

#ifndef BNBSCHEDULER_HPP
#define	BNBSCHEDULER_HPP

#include <vector>
#include "solverinfo.hpp"

#define MAX_ARGS 4


class BNBScheduler {
public:

    /**
     * External events that affect the scheduler
     */
    struct Events {
        /**
         * Operation finished erroneously
         */
        static const int ERROR = -1;
        /**
         * Dummy event
         */
        static const int DUMMY_EVENT = 0;
        /**
         * Initialization
         */
        static const int START = 1;
        /**
         * The requested portion of work done
         * arguments: actual number of steps performed
         */
        static const int DONE = 2;
        /**
         * The requested data sent to the server
         */
        static const int SENT = 3;
        /**
         * The requested data wasn't sent to the server because there is nothing to send
         */
        static const int NOTHING_TO_SEND = 4;
        /**
         * Data arrived (subproblems, records or both)
         * arguments: the  process that sent the data
         */
        static const int DATA_ARRIVED = 5;
        /**
         * Command arrived
         * arguments: the  process that sent the command and the command
         */
        static const int COMMAND_ARRIVED = 6;
        /**
         * Search strategy was successfully set by the solver
         */
        static const int SEARCH_STRATEGY_SET = 7;
        /**
         * Heuristic was successfully set by the solver
         */
        static const int HEURISTIC_SET = 8;
        /**
         * Bounding method was successfully set by the solver
         */
        static const int BOUNDING_METHOD_SET = 9;
    };

    /**
     * Structure to hold event information
     */
    struct Event {
        Event() {
            for(int i = 0; i < MAX_ARGS; i ++) {
                mArgs[i] = 0;
            }
        }
        /**
         * Event code
         */
        int mCode;
        /**
         * Event arguments
         */
        long long mArgs[MAX_ARGS];
    };

    /**
     * Commands issued by the scheduler
     */
    struct Actions {
        /**
         * Dummy action
         */
        static const int DUMMY_ACTION = 0;
        /**
         * Send subproblems 
         * arguments:  destination process and number of subproblems
         */
        static const int SEND_SUB = 1;
        /**
         * Send records
         * arguments: destination process
         */
        static const int SEND_RECORDS = 2;
        /**
         * Send subproblems and records in one message 
         * arguments: destination process and number of subproblems
         */
        static const int SEND_SUB_AND_RECORDS = 3;
        /**
         * Send command
         * arguments: command and destination process
         */
        static const int SEND_COMMAND = 4;
        /**
         * Recieve information (go for waiting loop)
         * arguments: number of process to wait for information, -1 for template
         */
        static const int RECV = 5;
        /**
         * Set search strategy (usually depth-first, best-first or custom)
         * argument: the search strategy
         */
        static const int SET_SEARCH_STRATEGY = 6;
        /**
         * Set heuristic to use. Heuristics are numbered 1, 2, 3, ...
         * The highest number means more powerful and more
         * resource consuming heuristic
         */
        static const int SET_HEURISTIC = 7;
        /**
         * Set bounding method. Sets the strategy to find the lower bound and/
         * or reduce the current subproblem
         */
        static const int SET_BOUNDING_METHOD = 8;
        /**
         * Perform several steps of the resolution method
         * arguments: number of steps to perform
         */
        static const int SOLVE = 9;
        /**
         * Exit the process
         */
        static const int EXIT = 10;
    };

    /**
     * Struct to hold action information
     */
    struct Action {
        Action() {
            for(int i = 0; i < MAX_ARGS; i ++) {
                mArgs[i] = 0;
            }
        }
        /**
         * Action code
         */
        int mCode;
        /**
         * Action args
         */
        long long mArgs[MAX_ARGS];
    };

    /**
     * Setup process rank
     * @param rank rank to set
     */
    void setRank(int rank) {
        mRank = rank;
    }

    /**
     * Setup communicator size (number of processes)
     * @param size
     */
    void setSize(int size){
        mSize = size;
    }

    /**
     * Take an action upon an event
     * @param event incoming event
     * @param info information about the solver state
     * @param action action to take
     */
    virtual void action(const Event& event, const SolverInfo& info, Action& action)  = 0;

    /**
     * The rank (number) of the current process
     */
    int mRank;

    /**
     * The total number of processes
     */
    int mSize;

};


#endif	/* BNBSCHEDULER_HPP */

