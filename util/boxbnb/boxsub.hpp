/* 
 * File:   boxsub.hpp
 * Author: medved
 *
 * Box based sub-problems for box based BnB method
 * Created on January 3, 2015, 10:50 PM
 */

#ifndef BOXSUB_HPP
#define	BOXSUB_HPP

#include <sstream>
#include <string>
#include <util/box/box.hpp>
#include <util/box/boxutils.hpp>

/**
 * The sub-problem for box-based BnB methods
 */
template <typename FT> struct BoxSub {

    /**
     * The constructor
     * @param dim box dimension
     * @param depth tree depth
     */
    BoxSub(int dim, int depth = 0) :
    mBox(dim), mDepth(depth) {
    }

    /**
     * Make a string representation
     * @return string representation
     */
    std::string toString() const {
        std::ostringstream os;
        std::string s = BoxUtils::toString(mBox);
        os << s << ", dep = " << mDepth;
        return os.str();
    }
    
    /**
     * Hyper interval of the subproblem
     */
    Box<FT> mBox;

    /**
     * Depth of the subproblem in the branching tree
     */
    int mDepth;

};


#endif	/* BOXSUB_HPP */

