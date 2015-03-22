/* 
 * File:   cutapplicator.hpp
 * Author: medved
 *
 * Abstract class for application of cuts to a box
 * 
 * Created on December 21, 2014, 12:53 PM
 */

#ifndef CUTAPPLICATOR_HPP
#define	CUTAPPLICATOR_HPP

#include <vector>
#include "cut.hpp"
#include "util/box/box.hpp"

/**
 * Abstract class for applying cuts to a box
 */
template <class FT> class CutApplicator {
    public:
        /**
         * Applies a set of cuts to a box
         * @param cuts a vector of cuts
         * @param box a box to apply cuts
         * @param v the vector to append resulting boxes
         */
        virtual void ApplyCut(const std::vector< Cut<FT> > &cuts, const Box<FT> &box, std::vector< Box<FT> > &v) = 0;
};

#endif	/* CUTAPPLICATOR_HPP */

