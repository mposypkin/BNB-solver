/* 
 * File:   mpsimpbagser.hpp
 * Author: posypkin
 *
 * Serialization of the simple bag of points
 *
 * Created on September 5, 2011, 11:35 AM
 */

#ifndef MULTISER_HPP
#define	MULTISER_HPP

#include <problems/pareto/multiopt/point.hpp>
#include <problems/pareto/multiopt/multipoint.hpp>
#include <problems/pareto/multiopt/mpsimpbag.hpp>
#include <util/mplib/binary_serializer.hpp>

template <> void BinarySerializer::pack(const Point<double> &data) {
    int n = data.mDim;
    pack(n);
    for (int i = 0; i < n; i++) {
        pack(data.mX[i]);
    }

}

template <> void BinarySerializer::unpack(Point<double> &data) {
    int n = 10;
    unpack(n);
    BNB_ASSERT(n == data.mDim);
    for (int i = 0; i < n; i++) {
        unpack(data.mX[i]);
    }
}

template <> void BinarySerializer::pack(const MultiPoint<double> &data) {
    pack(data.mParam);
    pack(data.mCrit);
}

template <> void BinarySerializer::unpack(MultiPoint<double> &data) {
    unpack(data.mParam);
    unpack(data.mCrit);
}

template <> void BinarySerializer::pack(const MPSimpBag<double> &data) {
    int nparam = data[0].mParam.mDim;
    int ncrit = data[0].mCrit.mDim;
    pack(nparam);
    pack(ncrit);
    int n = data.size();
    pack(n);
    for (int i = 0; i < n; i++) {
        pack(data[i]);
    }
}

template <> void BinarySerializer::unpack(MPSimpBag<double> &data) {
    int nparam;
    int ncrit;
    int n;
    unpack(nparam);
    unpack(ncrit);
    unpack(n);
    for (int i = 0; i < n; i++) {
        MultiPoint<double> mp(nparam, ncrit);
        unpack(mp);
        data.put(mp);
    }
}



#endif	/* MULTISER_HPP */

