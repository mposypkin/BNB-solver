/* 
 * File:   boxser.hpp
 * Author: posypkin
 *
 * Serialization of boxes and lists of boxes
 * 
 * Created on September 6, 2011, 1:53 PM
 */

#ifndef BOXSER_HPP
#define	BOXSER_HPP


#include "box.hpp"
#include "simplboxbag.hpp"
#include <util/mplib/binary_serializer.hpp>

template <> void BinarySerializer::pack(const Box<double> &data) {
    int n = data.mDim;
    pack(n);
    for (int i = 0; i < n; i++) {
        pack(data.mA[i]);
    }
    for (int i = 0; i < n; i++) {
        pack(data.mB[i]);
    }

}

template <> void BinarySerializer::unpack(Box<double> &data) {
    int n = 10;
    unpack(n);
    BNB_ASSERT(n == data.mDim);
    for (int i = 0; i < n; i++) {
        unpack(data.mA[i]);
    }
    for (int i = 0; i < n; i++) {
        unpack(data.mB[i]);
    }
}

template <> void BinarySerializer::pack(const SimpBoxBag<double> &data) {
    int n = data.size();
    BNB_ASSERT(n > 0);
    int dim = data[0].mDim;
    pack(n);
    pack(dim);
    for (int i = 0; i < n; i++) {
        pack(data[i]);
    }
}

template <> void BinarySerializer::unpack(SimpBoxBag<double> &data) {
    int n;
    unpack(n);
    int dim;
    unpack(dim);
    for (int i = 0; i < n; i++) {
        Box<double> b(dim);
        unpack(b);
        data.put(b);
    }
}

#endif	/* BOXSER_HPP */

