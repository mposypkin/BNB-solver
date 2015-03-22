/* 
 * File:   ineqsolver.hpp
 * Author: posypkin
 *
 * Base class for Inequality solvers
 * Created on April 25, 2011, 11:59 AM
 */

#ifndef INEQSOLVER_HPP
#define	INEQSOLVER_HPP

template <typename FT> class IneqSolver {

public:
    /**
     * Set Inequality matrix
     * @param nv number of variables
     * @param nineq number of inequalities
     * @param data inequality coefficients (nv + 1) x nineq array (the last is constant)
     */
    virtual void setInequalities(int nv, int nineq, FT* data) = 0;


    /**
     * Finds a feasible direction according to the inequalities
     * @param p the found vector
     * @return true if such vector exists false otherwise
     */
    virtual bool findFeasible(FT* p) = 0;


};

#endif	/* INEQSOLVER_HPP */

