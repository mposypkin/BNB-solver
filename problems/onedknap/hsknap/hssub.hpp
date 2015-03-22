/* 
 * File:   hssub.hpp
 * Author: posypkin
 *
 * Class to host subproblems of HS BnB method
 *
 * Created on August 25, 2012, 10:21 AM
 */

#ifndef HSSUB_HPP
#define	HSSUB_HPP

#include "util/common/smartptr.hpp"

/**
 * subproblem for HS method
 */
template <class T> struct HSSub {

    /**
     * Cumulative weight of all items currently in knapsack
     */
    T mW;

    /**
     * Cumulative price of all items currently in knapsack
     */
    T mP;

    /**
     * Cumulative weight of free items
     */
    T mFW;

    /**
     * Cumulative price of free items
     */
    T mFP;

    /**
     * Vector of assigned values
     */
    SmartArrayPtr<T> mX;

    /**
     * Number of "frozen" variables - varaibles those won't change at any circumnstaces
     */
    int mFrozen;

    /**
     * The current "head" position (next item to explore)
     */
    int mH;
};


#endif	/* HSSUB_HPP */

