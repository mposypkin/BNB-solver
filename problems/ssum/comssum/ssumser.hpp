/* 
 * File:   ssumser.hpp
 * Author: posypkin
 *
 * The serializer for the subset-sum problem definition
 *
 * Created on April 24, 2012, 8:13 PM
 */

#ifndef SSUMSER_HPP
#define	SSUMSER_HPP

#include <util/mplib/binary_serializer.hpp>
#include "ssum.hpp"

template <> void BinarySerializer::pack(const SSum<int> &data) {
    pack(data.mDim);
    pack(data.mCapacity);
    for(int i = 0; i < data.mDim; i ++) {
        pack(data.mWeights[i]);
    }
}

template <> void BinarySerializer::unpack(SSum<int> &data) {
    unpack(data.mDim);
    unpack(data.mCapacity);
    data.mWeights.realloc(data.mDim);
    for(int i = 0; i < data.mDim; i ++) {
        int w;
        unpack(w);
        data.mWeights[i] = w;
    }
}

#endif	/* SSUMSER_HPP */

