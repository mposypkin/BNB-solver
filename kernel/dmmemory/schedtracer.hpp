/* *
 * File:   schedtracer.hpp
 * Author: posypkin
 *
 * Storing and printing scheduler traces
 *
 * Created on September 14, 2011, 1:12 PM
 */

#ifndef SCHEDTRACER_HPP
#define	SCHEDTRACER_HPP

#include <new>
#include <string>
#include <sstream>
#include "util/common/bnberrcheck.hpp"
#include "bnbscheduler.hpp"
#include "tracer.hpp"


#define TRACE_RECORD_SIZE 14

#define STD_TRACE "STD"

class SchedTracer : public Tracer{
public:
    typedef long long int Record[TRACE_RECORD_SIZE];

    /**
     * Constructor
     * @param bufsize the number of records in the buffer
     */
    SchedTracer(int bufsize, char* traceFolder = STD_TRACE) {
        try {
            mBuf = new Record[bufsize];
            mBufSize = bufsize;
            mCnt = 0;
            mTraceFolder = traceFolder;
        } catch (std::bad_alloc& ba) {
            BNB_ERROR_REPORT("Failed to allocate the tracer's buffer\n");
        }

    }

    ~SchedTracer() {
        delete[] mBuf;
    }

    /**
     * Trace the scheduler  action
     * @param tp time point
     * @param rank process number to trace
     * @param event incoming event
     * @param info information about the solver state
     * @param action action to take
     */
    void traceAction(long long int tp, int rank, BNBScheduler::Action & action, BNBScheduler::Event& event, const SolverInfo& info) {
        int i = 0;
        int j;
        mBuf[mCnt][i++] = tp;
        mBuf[mCnt][i++] = rank;
        mBuf[mCnt][i++] = action.mCode;
        for (j = 0; j < MAX_ARGS; j++) {
            mBuf[mCnt][i++] = action.mArgs[j];
        }
        mBuf[mCnt][i++] = event.mCode;
        for (j = 0; j < MAX_ARGS; j++) {
            mBuf[mCnt][i++] = event.mArgs[j];
        }

        mBuf[mCnt][i++] = info.mNSub;
        mBuf[mCnt][i++] = (info.mRecUpdated) ? 1 : 0;
        mCnt++;
        if (mCnt >= mBufSize) {
            flush(rank);
        }
    }

    void flush(int rank) {
        FILE* f;
        if (mTraceFolder == STD_TRACE) {
            f = stdout;
        } else {
            std::ostringstream os;
            os << mTraceFolder;
            os << "/" << rank;
            f = fopen(os.str().c_str(), "a");
            BNB_ASSERT(f);
        }

        for (int i = 0; i < mCnt; i++) {
            for (int j = 0; j < TRACE_RECORD_SIZE; j++) {
                fprintf(f, "%lld ", mBuf[i][j]);
            }
            fprintf(f, "\n");
        }
        fflush(f);
        mCnt = 0;
        if (mTraceFolder != STD_TRACE) {
            fclose(f);
        }
    }

private:
    std::string mTraceFolder;
    Record* mBuf;
    int mCnt;
    int mBufSize;
};




#endif	/* SCHEDTRACER_HPP */

