/* 
 * File:   ssumsubser.hpp
 * Author: posypkin
 *
 * Created on April 25, 2012, 8:25 PM
 */

#ifndef SSUMSUBSER_HPP
#define	SSUMSUBSER_HPP

#include <util/mplib/binary_serializer.hpp>
#include "ssumsub.hpp"

template <> void BinarySerializer::pack(const SSumSub<int> &data) {
    int n = data.mValues.size();
    pack(n);
    pack(data.mFixedItems);
    pack(data.mRemCapacity);
    pack(data.mRemWeight);
    for(int i = 0; i < n; i ++) {
        pack(data.mValues[i]);
    }
}

template <> void BinarySerializer::unpack(SSumSub<int> &data) {
    int n;
    unpack(n);
    unpack(data.mFixedItems);
    unpack(data.mRemCapacity);
    unpack(data.mRemWeight);
    data.mValues.realloc(n);
    for(int i = 0; i < n; i ++) {
        char w;
        unpack(w);
        data.mValues[i] = w;
    }
}


#endif	/* SSUMSUBSER_HPP */

