/* 
 * File:   simplboxbag.hpp
 * Author: posypkin
 * The simplistic bag of boxes that can be used for different purposes
 *
 * Created on April 28, 2011, 10:10 AM
 */

#ifndef SIMPLBOXBAG_HPP
#define	SIMPLBOXBAG_HPP

#include <vector>
#include <sstream>
#include "box.hpp"
#include "boxutils.hpp"

/**
 * The simplistic bag of boxes
 */
template <typename FT> class SimpBoxBag {
public:

    /**
     * Put a box to the list
     * @param box
     */
    void put(const Box<FT>& box) {
        mBag.push_back(box);
    }

    /**
     * Returns and removes one box from a bag
     * @return box
     */
    Box<FT> get() {
        Box<FT> b = mBag.back();
        mBag.pop_back();
        return b;
    }

    /**
     * Returns size of the bag
     * @return size
     */
    int size() const {
        return mBag.size();
    }

    /**
     * Checks whether the bag is empty
     * @return true if the bag is empty
     */
    bool empty() const {
        return mBag.empty();
    }

    /**
     * Returns string representation 
     * @return string represnetaion
     */
    std::string toString() const {
        std::ostringstream os;
        typename std::vector< Box<FT> >::const_iterator i;
        for(i = mBag.begin(); i != mBag.end(); i ++) {
            os << BoxUtils::toString(*i);
        }
        return os.str();
    }

    /**
     * Retrieve the i-th element in a bag
     *
     * @param i element index
     * @return element
     */
    Box<FT> & operator [] (int i) {
        return mBag[i];
    }

    /**
     * Retrieve the i-th element in a bag
     *
     * @param i element index
     * @return element
     */
    const Box<FT> & operator [] (int i) const {
        return mBag[i];
    }


private:

    std::vector< Box<FT> > mBag;
    
};



#endif	/* SIMPLBOXBAG_HPP */

