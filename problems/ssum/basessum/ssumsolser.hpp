/* 
 * File:   ssumsubser.hpp
 * Author: posypkin
 *
 * Created on April 25, 2012, 8:25 PM
 */

#ifndef SSUMSOLSER_HPP
#define	SSUMSOLSER_HPP

#include <util/mplib/binary_serializer.hpp>
#include "ssumsol.hpp"

template <> void BinarySerializer::pack(const SSumSolution<int> &data) {
    int n = data.mValues.size();
    pack(n);
    pack(data.mValue);
    for(int i = 0; i < n; i ++) {
        pack(data.mValues[i]);
    }
}

template <> void BinarySerializer::unpack(SSumSolution<int> &data) {
    int n;
    unpack(n);
    unpack(data.mValue);
    data.mValues.realloc(n);
    for(int i = 0; i < n; i ++) {
        char w;
        unpack(w);
        data.mValues[i] = w;
    }
}


#endif	/* SSUMSOLSER_HPP */

