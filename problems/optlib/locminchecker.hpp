/* 
 * File:   locminchecker.hpp
 * Author: Mikhail Posypkin
 *
 * Created on June 10, 2014, 5:21 PM
 * The file aimed at checking whether a given point local minima
 * for box-constrained problems
 */

#ifndef LOCMINCHECKER_HPP
#define	LOCMINCHECKER_HPP

#include <util/box/box.hpp>
#include <util/spacefill/boxfill.hpp>
#include <util/spacefill/rndfill.hpp>

#include "objective.hpp"

/**
 * Checks whether a point local minimum in a given box neighborhood
 * for unconstrained problems
 */
template <typename FT> class LocalMinChecker {
public:
    /**
     * Constructor
     * @param box
     */
    LocalMinChecker(Objective<FT>* obj):
    mObj(obj)
    {
    }
    /**
     * Checks whether the given point is a local minimum
     * @param x local min point 
     * @param y placeholder for a better point
     * @param nt number of tries
     * @param box local neibourhood
     * @return true if no less points found, found point is copied to y
     */
    bool check(FT* x, FT* y, int nt, const Box<FT>& box) {
        RndFill<FT> rf(box);
        bool rv = true;
        FT v = mObj->func(x);
        for(int i = 0; i < nt; i ++) {
            rf.getPoint(y);
            if(mObj->func(y) < v) {
                rv = false;
                break;
            }
        }
        return rv;
    }
    
private:
    Objective<FT>* mObj;
    
};


#endif	/* LOCMINCHECKER_HPP */

