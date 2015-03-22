/* 
 * File:   ssumutil.hpp
 * Author: posypkin
 *
 * Created on April 20, 2012, 11:52 AM
 */

#ifndef SSUMUTIL_HPP
#define	SSUMUTIL_HPP

#include <sstream>
#include <util/common/utilmacro.hpp>
#include "ssum.hpp"

class SSumUtil {
public:
    /**
     * Reads the input data from string
     * format: N C w1 w2 ... wN
     * a nd produces the resulting problem
     * @param s string
     */
    template <class VT> static SSum<VT>* fromString(std::string& s) {
        std::istringstream is(s);
        int n;
        is >> n;
        SSum<VT>* pr = new SSum<VT>(n);
        BNB_ASSERT(pr);
        is >> pr->mCapacity;
        for (int i = 0; i < n; i++) {
            is >> pr->mWeights[i];
        }
        return pr;
    }

};

#endif	/* SSUMUTIL_HPP */

