/* 
 * File:   ssumsub.hpp
 * Author: posypkin
 *
 * Subproblem for subset sum
 *
 * Created on April 12, 2012, 4:35 PM
 */

#ifndef SSUMSUB_HPP
#define	SSUMSUB_HPP

#include <util/common/smartptr.hpp>
#include <util/mplib/binary_serializer.hpp>


template <class VT> struct SSumSub {

    /**
     * The constructor
     * @param dim the number of items
     */
    SSumSub(int dim) : mValues(dim) {
    }

    /**
     * The remaining weight
     */
    VT mRemWeight;

    /**
     * The remaining capacity
     */
    VT mRemCapacity;

    /**
     * The number of fixed items
     */
    int mFixedItems;

    /**
     * The set of xi values
     */
    SmartArrayPtr<char> mValues;
};



#endif	/* SSUMSUB_HPP */

