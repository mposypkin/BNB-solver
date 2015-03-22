/* 
 * File:   cutfactory.hpp
 * Author: medved
 *
 * Created on November 1, 2014, 9:06 PM
 */

#ifndef CUTFACTORY_HPP
#define	CUTFACTORY_HPP

#include <vector>

#include <util/box/box.hpp>
#include "cut.hpp"

/**
 * Interface for generating new cuts
 */
template <class FT> class CutFactory {
    public:
        /**
         * Get a cut for a given box
         * 
         * @param box box to produce cuts
         * @param cuts vector to append produced cuts
         */
        virtual void getCuts(const Box<FT>& box, std::vector< Cut<FT> >& cuts) = 0;
};

#endif	/* CUTFACTORY_HPP */

