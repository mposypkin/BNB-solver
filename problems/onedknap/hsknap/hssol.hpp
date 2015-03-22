/* 
 * File:   hssol.hpp
 * Author: posypkin
 *
 * Clas for storing final knapsack solutions
 *
 * Created on August 25, 2012, 5:35 PM
 */

#ifndef HSSOL_HPP
#define	HSSOL_HPP

#include <util/common/smartptr.hpp>

template <class T> struct HSSol {
    /**
     * Cumulative price of all items currently in knapsack
     */
    T mP;

    /**
     * Vector of assigned values
     */
    SmartArrayPtr<T> mX;

};

#endif	/* HSSOL_HPP */

