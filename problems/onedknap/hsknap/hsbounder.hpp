/* 
 * File:   bounder.hpp
 * Author: posypkin
 *
 * Abstract class for bounder (to use by HS decomposition)
 *
 * Created on August 27, 2012, 10:40 AM
 */

#ifndef HSBOUNDER_HPP
#define	HSBOUNDER_HPP

#include "problems/onedknap/common/knapdata.hpp"
#include "hssub.hpp"
#include "hssol.hpp"

template <class T> class HSBounder {
public:
    /**
     * Evaliates lower and upper bounds and checks whether the problem should be discarded
     * @param kd the problem definition
     * @param sub the subproblem
     * @param record the best solution found so far (updated if needed)
     * @return true if the problem should be discarded and false otherwise
     */
    virtual bool BoundAndReduce(const KnapData<T>& kd, const HSSub<T>& sub, HSSol<T>& record) = 0;
    
};


#endif	/* BOUNDER_HPP */

