/* 
 * File:   ssum.hpp
 * Author: posypkin
 *
 * Created on April 12, 2012, 11:20 AM
 * Problem statement (coefficients) for subset sum problem
 */

#ifndef SSUM_HPP
#define	SSUM_HPP
#include <util/common/smartptr.hpp>
template <class VT> struct SSum {   
    
    SSum(int dim) : mWeights(dim)
    {
        mDim = dim;
    }

 
    /**
     * The weights (equal to prices) in subset sum
     */
    SmartArrayPtr<VT> mWeights;

    /**
     * The capacity
     */
    VT mCapacity;

    /**
     * The number of items
     */
    int mDim;    
};



#endif	/* SSUM_HPP */

