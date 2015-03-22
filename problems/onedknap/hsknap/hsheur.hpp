/* 
 * File:   hsheur.hpp
 * Author: posypkin
 *
 * An abstract file to manage heuristics 
 *
 * Created on October 7, 2012, 1:38 PM
 */

#ifndef HSHEUR_HPP
#define	HSHEUR_HPP

#include "hssol.hpp"
#include "hssub.hpp"

template <class T> class HSHeur {
public:
    /**
     * Search for a solution better than existing one
     * @param sub subproblem to search in
     * @param sol the resulting solution (if improved)
     * @param nsteps number of steps (updated on exit)
     * @return true if the value was improved
     */
    virtual bool search(const HSSub<T>& sub, HSSol<T>& sol, long long& nsteps) = 0;
};

#endif	/* HSHEUR_HPP */

