/* 
 * File:   hscompare.hpp
 * Author: posypkin
 *
 * Created on September 12, 2012, 6:56 PM
 */

#ifndef HSCOMPARE_HPP
#define	HSCOMPARE_HPP

#include "hssub.hpp"

template <class T> class HSCompare {
public:
    bool operator() (const HSSub<T>& s1, const HSSub<T>& s2) const {
        return (s1.mFrozen < s2.mFrozen);
    }
};


#endif	/* HSCOMPARE_HPP */

