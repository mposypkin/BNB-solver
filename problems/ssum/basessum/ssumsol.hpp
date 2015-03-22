/* 
 * File:   ssumsol.hpp
 * Author: posypkin
 *
 * Subset sum solution
 *
 * Created on April 12, 2012, 5:38 PM
 */

#ifndef SSUMSOL_HPP
#define	SSUMSOL_HPP

#include <util/common/smartptr.hpp>

template<class VT> struct SSumSolution {
    /**
     * The constructor
     * @param dim the number of items
     */
    SSumSolution(int dim) : mValues(dim)
    {
        mValue = 0;
    }
    
    /**
     * The value of the objective
     */
    VT mValue;
    
    /**
     * The set of xi values
     */
    SmartArrayPtr<char> mValues;    
};


#endif	/* SSUMSOL_HPP */

