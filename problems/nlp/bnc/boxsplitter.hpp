/* 
 * File:   boxsplitter.hpp
 * Author: medved
 *
 * Created on June 8, 2015, 3:17 PM
 * 
 * Generic class to split the box
 */

#ifndef BOXSPLITTER_HPP
#define	BOXSPLITTER_HPP

#include <vector>
#include <util/box/box.hpp>

template <class FT> class BoxSplitter {
    public:
        /**
         * Splits the box into several smaller boxes
         * @param box box to split
         * @param boxv the vector to push resulting boxes
         */
        virtual void split(const Box<FT>& box, std::vector< Box<FT> >& boxv) = 0;
};


#endif	/* BOXSPLITTER_HPP */

