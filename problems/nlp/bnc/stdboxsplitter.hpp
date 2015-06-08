/* 
 * File:   stdboxsplitter.hpp
 * Author: medved
 *
 * Created on June 8, 2015, 3:22 PM
 */

#ifndef STDBOXSPLITTER_HPP
#define	STDBOXSPLITTER_HPP

#include <util/box/boxutils.hpp>
#include "boxsplitter.hpp"

/**
 * Standard box splitter
 */
template <class FT> class StdBoxSplitter : public BoxSplitter<FT> {

    void split(const Box<FT>& box, std::vector< Box<FT> >& boxv) {
        int n = box.mDim;
        Box<FT> b1(n), b2(n);
        BoxUtils::divideByLongestEdge(box, b1, b2);
        boxv.push_back(b1);
        boxv.push_back(b2);
    }
};

#endif	/* STDBOXSPLITTER_HPP */

