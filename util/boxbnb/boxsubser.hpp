/* 
 * File:   boxsubser.hpp
 * Author: medved
 *
 * Created on January 5, 2015, 10:59 PM
 */

#ifndef BOXSUBSER_HPP
#define	BOXSUBSER_HPP

#include <util/mplib/binary_serializer.hpp>
#include <util/box/boxser.hpp>
#include "boxsub.hpp"

template <> void BinarySerializer::pack(const BoxSub<double> &data) {
    pack(data.mDepth);
    pack(data.mBox);
}

template <> void BinarySerializer::unpack(BoxSub<double> &data) {
    unpack(data.mDepth);
    unpack(data.mBox);
}


#endif	/* BOXSUBSER_HPP */

