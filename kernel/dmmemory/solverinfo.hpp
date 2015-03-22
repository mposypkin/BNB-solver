/* 
 * File:   solverinfo.hpp
 * Author: posypkin
 *
 * Contains various information about solver state
 *
 * Created on September 11, 2011, 12:08 AM
 */

#ifndef SOLVERINFO_HPP
#define	SOLVERINFO_HPP

/**
 * This strucuture holds information about the solve state
 */
struct SolverInfo {
    /**
     * Number of subproblems
     */
    long long mNSub;

    /**
     * True if record was updated since the last reset
     */
    bool mRecUpdated;
};




#endif	/* SOLVERINFO_HPP */

