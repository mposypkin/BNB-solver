/* 
 * File:   bnbresolver.hpp
 * Author: posypkin
 *
 * The generic class for Branch-and-Bound Solvers
 * Created on September 10, 2011, 1:11 PM
 */

#ifndef BNBRESOLVER_HPP
#define	BNBRESOLVER_HPP

#include <util/mplib/binary_serializer.hpp>
#include <util/common/bnberrcheck.hpp>
#include "solverinfo.hpp"

class BNBResolver {
public:
    /**
     * Get solver state info
     * @param info information structure to fill
     */
    virtual void getInfo(SolverInfo& info) {
        BNB_ERROR_REPORT("BNBResolver::getInfo method is not implemented");
    }

    /**
     * Reset solver state info
     */
    virtual void resetInfo() {
        BNB_ERROR_REPORT("BNBResolver::resetInfo method is not implemented");
    }
    /**
     * Push subsets to the serializer
     * 
     * @param num number of subsets (on exit - actual number of packed to sent)
     * @param ser the serializer
     */
    virtual void getSubs(int& num, BinarySerializer& ser){
        BNB_ERROR_REPORT("BNBResolver::getSubs method is not implemented");
    }
    /**
     * Push record(s) to the serializer
     * @param set the serilizer
     */
    virtual void getRecords(BinarySerializer& ser) {
        BNB_ERROR_REPORT("BNBResolver::getRecords method is not implemented");
    }

    /**
     * Take subproblems from the serializer and put them to the solver
     *
     * @param ser serializer
     * @return actual number of packed sub-problems
     */
    virtual int putSubs(BinarySerializer& ser) {
        BNB_ERROR_REPORT("BNBResolver::putSubs method is not implemented");
    }


    /**
     * Take record(s) from the serializer and put them to the solver
     *
     * @param ser serializer
     */
    virtual void putRecords(BinarySerializer& ser) {
                BNB_ERROR_REPORT("BNBResolver::putRecords method is not implemented");
    }

    /**
     * Setup search strategy
     * @param strategy the search strategy
     */
    virtual void setSearchStrategy(int strategy) {
        BNB_ERROR_REPORT("BNBResolver::setSearchStrategy method is not implemented");
    }

    /**
     * Setup heuristic
     * the given heuristic is implementation-specific
     * the bigger number the more complex and hopefully powerful heuristic
     * numbers start from 1 (0 means no heuristic)
     * @param heuristic a heuristic to set
     */
    virtual void setHeuristic(int heuristic) {
        BNB_ERROR_REPORT("BNBResolver::setHeuristic method is not implemented");
    }

    /**
     * Setup bounding method
     * the bigger number the more complex and powerful bounding method
     * @param method bounding method to set
     */
    virtual void setBoundingMethod(int method) {
        BNB_ERROR_REPORT("BNBResolver::setBoundingMethod method is not implemented");
    }

    /**
     * Perform several algorithmic steps (or less)
     * @param steps on entry: requested number of steps, on exit: actual number of steps
     *        -1 on exit means an error
     */
    virtual void solve(long long& steps)    {
        BNB_ERROR_REPORT("BNBResolver::solve method is not implemented");
    }



};


#endif	/* BNBRESOLVER_HPP */

