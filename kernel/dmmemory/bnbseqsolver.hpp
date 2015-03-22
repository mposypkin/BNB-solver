/* 
 * File:   bnbdmsolver.hpp
 * Author: posypkin
 *
 * A sequential solver
 *
 * Created on September 10, 2011, 11:04 AM
 */

#ifndef BNBSEQSOLVER_HPP
#define	BNBSEQSOLVER_HPP

#include <sstream>
#include <chrono>
#include <util/common/util.hpp>
#include "bnbscheduler.hpp"
#include "bnbresolver.hpp"
#include "tracer.hpp"
#include "counters.hpp"

class BNBSeqSolver {
public:

    /**
     * The constructor
     *
     * @param comm communicator
     * @param sched scheduler
     * @param solver solver
     */
    BNBSeqSolver(BNBScheduler* sched, BNBResolver* solver) {
        mSched = sched;
        mSolver = solver;
        mTracer = NULL;
        mStatsEnabled = false;
    }

    /**
     * Main process
     *
     * @param rank the process rank
     * @param size size of the communicator
     */
    void solve() {
        SolverInfo info;
        BNBScheduler::Action action;
        BNBScheduler::Event event;
        event.mCode = BNBScheduler::Events::START;
        std::chrono::time_point<std::chrono::system_clock> start;
        start = std::chrono::system_clock::now();
        mSolver->getInfo(info);
        mSched->action(event, info, action);
        long long int ms = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - start).count();
        trace(ms, action, event, info);
        bool quit = false;
        DmCounters cnt;
        Timer tim("");

        tim.start();
        while (true) {
            int code;
            int command;
            int dest;
            int num;
            int src;
            int typ;
            int r;
            long long n;
            switch (action.mCode) {
                case BNBScheduler::Actions::DUMMY_ACTION:
                    break;
                case BNBScheduler::Actions::SET_SEARCH_STRATEGY:
                    mSolver->setSearchStrategy(action.mArgs[0]);
                    event.mCode = BNBScheduler::Events::SEARCH_STRATEGY_SET;
                    break;
                case BNBScheduler::Actions::SET_HEURISTIC:
                    mSolver->setHeuristic(action.mArgs[0]);
                    event.mCode = BNBScheduler::Events::HEURISTIC_SET;
                    break;
                case BNBScheduler::Actions::SET_BOUNDING_METHOD:
                    mSolver->setBoundingMethod(action.mArgs[0]);
                    event.mCode = BNBScheduler::Events::BOUNDING_METHOD_SET;
                    break;
                case BNBScheduler::Actions::SOLVE:
                    n = action.mArgs[0];
                    mSolver->solve(n);
                    if (n < 0) {
                        event.mCode = BNBScheduler::Events::ERROR;
                    } else {
                        event.mCode = BNBScheduler::Events::DONE;
                        event.mArgs[0] = n;
                        cnt.steps_done += n;
                    }
                    break;
                case BNBScheduler::Actions::EXIT:
                    quit = true;
                    break;
                default:
                    printf("action [%d]\n", action.mCode);
                    BNB_ERROR_REPORT("Unrecognized action");
                    break;
            }
            if (quit) {
                mSolver->getInfo(info);
                //trace(action, event, info, true);
                tim.stop();
                std::ostringstream os;
                os << "time = " << tim.getTotalTime() << "\n";
                os << "0:" << cnt.toString() << "\n";
                writeStats(os.str().c_str());
                break;
            } else {
                mSolver->getInfo(info);
                mSched->action(event, info, action);
                ms = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - start).count();
                trace(ms, action, event, info);
            }
        }
    }

    /**
     * Setup tracer
     * @param st tracer
     */
    void setTracer(Tracer * st) {
        mTracer = st;
    }

    /**
     * Set file to output statistics
     * @param fname file name to output statistics
     */
    void setStatsFileName(std::string fname) {
        mStatFileName = fname;
    }

    /**
     * Enable or disable statistic printing
     * @param enabled true is stats should be enabled
     */
    void enableStats(bool enabled) {
        mStatsEnabled = enabled;
    }
private:

    void trace(long long int tp, BNBScheduler::Action action, BNBScheduler::Event& event, SolverInfo& info, bool last = false) {
        if (mTracer) {
            mTracer->traceAction(tp, 0, action, event, info);
        }
        if (last) {
            if (mTracer) {
                mTracer->flush(0);
            }
        }
    }

    void writeStats(const std::string& s) {
        if (mStatsEnabled) {
            FILE* f;
            if (mStatFileName.empty()) {
                f = stdout;
            } else {
                f = fopen(mStatFileName.c_str(), "a");
            }
            fprintf(f, "%s\n", s.c_str());
            if (f != stdout) {
                fclose(f);
            }
        }

    }

    bool mStatsEnabled;
    Tracer* mTracer;
    BNBScheduler* mSched;
    BNBResolver* mSolver;
    std::string mStatFileName;
};

#endif	/* BNBDMSOLVER_HPP */

