/* 
 * File:   cut.hpp
 * Author: medved
 * 
 * Class to store cuts 
 * 
 * Created on August 24, 2014, 4:07 PM
 */

#ifndef CUT_HPP
#define	CUT_HPP

#include <util/box/box.hpp>


template <class FT> struct Cut {
public:

    struct CutType {
        /**
         * Total cut: eliminate the whole set
         */
        static const unsigned int TOTAL = 1;
        /**
         * Cut the internity of the ball
         */
        static const unsigned int INNER_BALL = 2;

        /**
         * Cut the area outside the ball
         */
        static const unsigned int OUTER_BALL = 3;

        /**
         * Linear cut 
         */
        static const unsigned int LINEAR = 4;
    };

    
    /**
     * If the cut is a ball - it's ball radius. If the cut is 
     * hyperplane c^t x + r >= 0 it is 'r'
     */
    FT mR;
    
    /**
     * When cut is a ball this vector is a ball's center 'c'
     * When the cut is a hyperplane c^t x + r >= 0
     * mC  is a vector of coeafficients 'c', mR is r
     */
    SmartArrayPtr<FT> mC;

    /**
     * Type of the cut
     */
    unsigned int mType;

};


#endif	/* CUT_HPP */

