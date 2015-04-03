/* 
 * File:   bnbdmsolver.hpp
 * Author: posypkin
 *
 * A solver for distributed memory
 *
 * Created on September 10, 2011, 11:04 AM
 */

#ifndef BNBDMSOLVER_HPP
#define	BNBDMSOLVER_HPP

#include <sstream>
#include <chrono>
#include <util/mplib/communicator.hpp>
#include <util/common/util.hpp>
#include "bnbscheduler.hpp"
#include "bnbresolver.hpp"
#include "tracer.hpp"
#include "counters.hpp"

/*
 * Main Distibuted memory driver
 */
class BNBDmSolver {
public:

    /**
     * Message types
     */
    struct MessageTypes {
        /**
         * Command message
         */
        static const int COMMAND = 1;
        /**
         * A message with subproblems
         */
        static const int SUBPROBLEMS = 2;
        /**
         * A message with records
         */
        static const int RECORDS = 3;
        /**
         * A message with records and subproblems
         */
        static const int SUB_AND_RECORDS = 4;
    };

    /**
     * The constructor
     *
     * @param comm communicator
     * @param sched scheduler
     * @param solver solver
     */
    BNBDmSolver(Communicator* comm, BNBScheduler* sched, BNBResolver* solver) {
        mComm = comm;
        mSched = sched;
        mSolver = solver;
        mTracer = NULL;
    }

    /**
     * Main process
     *
     * @param rank the process rank
     * @param size size of the communicator
     */
    void solve(int rank, int size) {
        SolverInfo info;
        BNBScheduler::Action action;
        BNBScheduler::Event event;
        event.mCode = BNBScheduler::Events::START;
        std::chrono::time_point<std::chrono::system_clock> start;
        start = std::chrono::system_clock::now();
        BinarySerializer binser;
        mSched->setRank(rank);
        mSched->setSize(size);
        mSolver->getInfo(info);
        mSched->action(event, info, action);
        long long int ms = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - start).count();
        trace(ms, rank, action, event, info);
        bool quit = false;
        DmCounters cnt;

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
                case BNBScheduler::Actions::SEND_COMMAND:
                    typ = MessageTypes::COMMAND;
                    dest = action.mArgs[0];
                    command = action.mArgs[1];                    
                    binser << typ;
                    binser << command;
                    for(int i = 0; i < MAX_ARGS; i ++) {
                        binser << action.mArgs[i];
                    }
                    mComm->send(binser, dest);
                    binser.reset();
                    event.mCode = BNBScheduler::Events::SENT;
                    cnt.sent_commands++;
                    cnt.sent_parcels++;
                    break;
                case BNBScheduler::Actions::SEND_SUB:
                    dest = action.mArgs[0];
                    num = action.mArgs[1];
                    if (info.mNSub > 0) {
                        typ = MessageTypes::SUBPROBLEMS;
                        binser << typ;
                        mSolver->getSubs(num, binser);
                        mComm->send(binser, dest);
                        binser.reset();
                        event.mCode = BNBScheduler::Events::SENT;
                        cnt.sent_parcels++;
                        cnt.sent_subs += num;
                    } else {
                        event.mCode = BNBScheduler::Events::NOTHING_TO_SEND;
                    }
                    break;
                case BNBScheduler::Actions::SEND_RECORDS:
                    dest = action.mArgs[0];
                    typ = MessageTypes::RECORDS;
                    binser << typ;
                    mSolver->getRecords(binser);
                    mComm->send(binser, dest);
                    binser.reset();
                    event.mCode = BNBScheduler::Events::SENT;
                    cnt.sent_parcels++;
                    cnt.sent_records++;
                    break;
                case BNBScheduler::Actions::SEND_SUB_AND_RECORDS:
                    dest = action.mArgs[0];
                    num = action.mArgs[1];
                    if (info.mNSub > 0) {
                        typ = MessageTypes::SUB_AND_RECORDS;
                        binser << typ;
                        mSolver->getRecords(binser);
                        mSolver->getSubs(num, binser);
                        mComm->send(binser, dest);
                        binser.reset();
                        event.mCode = BNBScheduler::Events::SENT;
                        cnt.sent_parcels++;
                        cnt.sent_records++;
                        cnt.sent_subs += num;

                    } else {
                        event.mCode = BNBScheduler::Events::NOTHING_TO_SEND;
                    }
                    break;
                case BNBScheduler::Actions::RECV:
                    src = action.mArgs[0];
                    r = mComm->recv(binser);
                    binser >> code;
                    cnt.recv_parcels++;
                    if (code == MessageTypes::COMMAND) {
                        event.mCode = BNBScheduler::Events::COMMAND_ARRIVED;
                        binser >> command;
                        event.mArgs[0] = r;
                        event.mArgs[1] = command;
                        for(int i = 2; i < MAX_ARGS; i ++) {
                            binser >> event.mArgs[i];
                        }
                        cnt.recv_commands++;
                    } else if (code == MessageTypes::RECORDS) {
                        event.mCode = BNBScheduler::Events::DATA_ARRIVED;
                        event.mArgs[0] = r;
                        mSolver->putRecords(binser);
                        cnt.recv_records++;
                    } else if (code == MessageTypes::SUBPROBLEMS) {
                        event.mCode = BNBScheduler::Events::DATA_ARRIVED;
                        event.mArgs[0] = r;
                        cnt.recv_subs += mSolver->putSubs(binser);
                    } else if (code == MessageTypes::SUB_AND_RECORDS) {
                        event.mCode = BNBScheduler::Events::DATA_ARRIVED;
                        event.mArgs[0] = r;
                        mSolver->putRecords(binser);
                        cnt.recv_subs += mSolver->putSubs(binser);
                    }
                    binser.reset();
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
                long long int ms = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - start).count();
                trace(ms, rank, action, event, info, true);
                cnt.recv_bytes = mComm->getRecvBytes();
                cnt.sent_bytes = mComm->getSentBytes();

                if (rank == 0) {
                    std::ostringstream os;
                    os << "0:" << cnt.toString() << "\n";
                    for (int i = 1; i < size; i++) {
                        mComm->recv(binser, i);
                        std::string s;
                        binser >> s;
                        os << i << ":" << s << "\n";
                        binser.reset();
                    }
                    writeStats(os.str().c_str());
                } else {
                    binser.reset();
                    binser << cnt.toString();
                    mComm->send(binser, 0);
                    binser.reset();
                }


                //printf("%d: [%s]\n", rank, cnt.toString().c_str());
                break;
            } else {
                mSolver->getInfo(info);
                mSched->action(event, info, action);
                long long int ms = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - start).count();
                trace(ms, rank, action, event, info);
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

private:

    void trace(long long int tp, int rank, BNBScheduler::Action action, BNBScheduler::Event& event, SolverInfo& info, bool last = false) {
        if (mTracer) {
            mTracer->traceAction(tp, rank, action, event, info);
        }
        if (last) {
            if (mTracer) {
                mTracer->flush(rank);
            }
        }
    }

    void writeStats(const std::string& s) {
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

    Tracer* mTracer;
    Communicator* mComm;
    BNBScheduler* mSched;
    BNBResolver* mSolver;
    std::string mStatFileName;
};

#endif	/* BNBDMSOLVER_HPP */

