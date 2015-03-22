/* 
 * File:   mutlistate.hpp
 * Author: posypkin
 *
 * The state for multi cover
 * Created on April 27, 2011, 9:18 AM
 */

#ifndef MUTLISTATE_HPP
#define	MUTLISTATE_HPP

#include <sstream>
#include <util/box/box.hpp>


/**
 * State to hold the list of subproblems and the list of points
 */
template <class PointBag, class SubBag> struct MultiState {

    std::string toString() const {
        std::ostringstream os;
        os << "points:\n";
        os << mPointBag.toString();
        os << "\nboxes:\n";
        os << mSubBag.toString();
        return os.str();
    }

    /**
     * A bag of points
     */
    PointBag mPointBag;

    /**
     * A bag of rectangles
     */
    SubBag mSubBag;

};


#endif	/* MUTLISTATE_HPP */

