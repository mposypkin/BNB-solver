/* 
 * File:   boxfill.hpp
 * Author: Mikhail Posypkin
 *
 * Generate random points in a box
 * Created on June 10, 2014, 5:42 PM
 */

#ifndef BOXFILL_HPP
#define	BOXFILL_HPP

#include "util/box/box.hpp"

template <class FT> class BoxFill {
    public:
        
        /**
         * Constructor
         * @param box box to generate points in
         */
        BoxFill(const Box<FT>& box):
        mBox(box)
        {          
        }
        
        /**
         * Get next generated point
         * @param x vector of point coordinates to fill in
         */
        virtual void getPoint(FT* x) = 0;
        
        
    protected:
        Box<FT> mBox;
};

#endif	/* BOXFILL_HPP */

