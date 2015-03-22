/* 
 * File:   knapdata.hpp
 * Author: posypkin
 *
 * Created on August 18, 2012, 11:34 AM
 * Structure to hold coefficients of the boolean one-d knapsack problem
 *
 */

#ifndef KNAPDATA_HPP
#define	KNAPDATA_HPP

/**
 * This is the record type for storing knapsack coefficients
 */
template <class T> struct KnapRecord {
    /**
     * Price
     */
    T mP;

    /**
     * Weight
     */
    T mW;

    /**
     * Efficiency
     */
    double mE;

    /**
     * "Old" (before sorting) number
     */
    int mI;
};

/**
 * T should be integral type
 */
template <class T> struct KnapData {

    /**
     * Number of items
     */
    int mN;

    /**
     * Capacity
     */
    T mC;

    /**
     * Total weight of all items
     */
    T mTW;

    /**
     * Total price of all items
     */
    T mTP;

    /**
     * Coefficients in a form p1 w1 p2 w2 ... pN wN
     */
    KnapRecord<T> *mCoe;
};


#endif	/* KNAPDATA_HPP */

