/* 
 * File:   tracer.hpp
 * Abstract Interface to the scheduler tracer
 * Created on September 21, 2012, 4:08 PM
 */

#ifndef TRACER_HPP
#define	TRACER_HPP

/**
 * Tracer class
 */
class Tracer {
public:
    /**
     * Trace the scheduler  action
     * @param tp time point
     * @param rank process number to trace
     * @param event incoming event
     * @param info information about the solver state
     * @param action action to take
     */
    virtual void traceAction(long long int tp, int rank, BNBScheduler::Action & action, BNBScheduler::Event& event, const SolverInfo& info) = 0;

    /**
     * Flushed the scheduler information to the output
     */
    virtual void flush(int rank) = 0;
};

#endif	/* TRACER_HPP */

