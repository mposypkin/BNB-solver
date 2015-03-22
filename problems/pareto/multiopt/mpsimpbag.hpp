/* 
 * File:   mpsimpbag.hpp
 * Author: posypkin
 *
 * Simple bag to store pareto points
 *
 * Created on April 19, 2011, 11:39 AM
 */

#ifndef MPSIMPBAG_HPP
#define	MPSIMPBAG_HPP

#include <vector>
#include <string>
#include <sstream>

#include "multipoint.hpp"

/**
 * Implements simple (non-concurrent) version of the Bag for holding
 * pareto points
 */
template <typename FT> class MPSimpBag {
public:

    /**
     * Retrieve the full bag (const version)
     * @return
     */
    const std::vector< MultiPoint<FT> >& getBagConst() const {
        return mBag;
    }

    /**
     * Retrieve the full bag (non-const version)
     * @return
     */
    std::vector< MultiPoint<FT> >& getBag() {
        return mBag;
    }

    /**
     * Retrieve the i-th element in a bag
     *
     * @param i element index
     * @return element
     */
    MultiPoint<FT> & operator [] (int i) {
        return mBag[i];
    }

    /**
     * Retrieve the i-th element in a bag
     * const version
     * @param i element index
     * @return element
     */
    const MultiPoint<FT> & operator [] (int i) const {
        return mBag[i];
    }


    /**
     * Checks whether the bag is empty
     * @return true if the bag is empty
     */
    bool empty() const {
        return (mBag.size() == 0);
    }
    /**
     * Retrive number of elements in a bag
     * @return number of elements
     */
    int size() const {
        return mBag.size();
    }

    /**
     * Returns the text representation of the bag
     * @return stringified bag
     */
    std::string toString() const {
        std::ostringstream os;
        int n = size();
        for (int i = 0; i < n; i++) {
            os << "<p>";
            os << mBag[i].toString();
            os << "</p>\n";
        }
        return os.str();
    }

    /**
     * Retrieve and remove from the bag one point
     * @return point
     */
    MultiPoint<FT> get() {
        MultiPoint<FT> mp = mBag.back();
        mBag.pop_back();
        return mp;
    }

    /**
     * Puts the next point to the bag
     *
     * @param point point to put
     * @return true if the point was added and false otherwise
     */
    bool put(MultiPoint<FT>& point) {
        typename std::vector< MultiPoint<FT> >::iterator i = mBag.begin();
        bool put = true;
        for (; i != mBag.end();) {
            if (point >= *i) {
                put = false;
                break;
            } else if (point <= *i) {
                i = mBag.erase(i);
            } else
                i++;
        }
        if (put)
            mBag.push_back(point);
        return put;
    }

    /**
     * Checks whether point is dominated by at least one point in a bag
     * 
     * @param point 
     * @return 
     */
    bool dominate(const Point<FT>& point) const {
        typename std::vector< MultiPoint<FT> >::const_iterator i = mBag.begin();
        bool dom = false;
        for (; i != mBag.end();) {
            if (point >= i->mCrit) {
                dom = true;
                break;
            } else if (point <= i->mCrit) {
                break;
            } else
                i++;
        }
        return dom;
    }

    /**
     * Clears the container
     */
    void clear() {
        mBag.clear();
    }
private:

    std::vector< MultiPoint<FT> > mBag;
};


#endif	/* MPSIMPBAG_HPP */

