/* *
 * File:   simptracer.hpp
 * Author: posypkin
 *
 * Simple tracer
 *
 */

#ifndef SIMPTRACER_HPP
#define	SIMPTRACER_HPP

#include <new>
#include "util/common/bnberrcheck.hpp"
#include "bnbscheduler.hpp"
#include "tracer.hpp"


#define TRACE_RECORD_SIZE 13

#define STD_TRACE "STD"

class SimpTracer : public Tracer{
public:
    typedef struct {
        /**
         * Total number of steps
         */
        int mNSteps;
    } TracerInfo;

    SimpTracer() {
        mTracerInfo.mNSteps = 0;
    }

    /**
     * Trace the scheduler  action
     * @param rank process number to trace
     * @param event incoming event
     * @param info information about the solver state
     * @param action action to take
     */
    void traceAction(int rank, BNBScheduler::Action & action, BNBScheduler::Event& event, const SolverInfo& info) {
        if(event.mCode == BNBScheduler::Events::DONE) {
            mTracerInfo.mNSteps += event.mArgs[0];
        }
    }

    void flush(int rank) {        
    }

    /**
     * Retrieves references to the tracer Info
     */
    TracerInfo& getInfo() {
        return mTracerInfo;
    }

private:
    
    TracerInfo mTracerInfo;
};




#endif	/* SIMPTRACER_HPP */

