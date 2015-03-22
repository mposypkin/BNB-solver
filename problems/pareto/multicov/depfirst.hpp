/* 
 * File:   depfirst.hpp
 * Author: posypkin
 *
 * Comparator for the Depth-First search
 *
 * Created on March 15, 2012, 9:54 AM
 */

#ifndef DEPFIRST_HPP
#define	DEPFIRST_HPP

#include "multisub.hpp"

template <class FT> class DepFirst {
public:
    bool operator() (const MultiSub<FT>& s1, const MultiSub<FT>& s2) const {
        return (s1.mDepth < s2.mDepth);
    }
};


#endif	/* DEPFIRST_HPP */

