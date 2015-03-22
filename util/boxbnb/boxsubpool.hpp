/* 
 * File:   boxsubpool.hpp
 * Author: medved
 * The pool of box-based sub-problems
 * Created on January 3, 2015, 10:52 PM
 */

#ifndef BOXSUBPOOL_HPP
#define	BOXSUBPOOL_HPP

#include <set>
#include <sstream>
#include "boxsub.hpp"


template <class FT> class BoxCmp {
public:
    bool operator() (const BoxSub<FT>& s1, const BoxSub<FT>& s2) const {
        return (s1.mDepth > s2.mDepth);
    }
};

template <typename FT> class BoxSubPool {
public:

    typedef std::multiset< BoxSub<FT>, BoxCmp<FT> > QType;

    struct Directions {
        /**
         * Pops always largerst element in a queue
         */
        static const int TAKE_UPPER = 1;
        /**
         * Pops always smallest element in a queue
         */
        static const int TAKE_LOWER = 2;
    };

    /**
     * Constructor
     */
    BoxSubPool() {
        mDirection = Directions::TAKE_UPPER;
    }

    /**
     * Put a subproblem to the list
     * @param box
     */
    void put(const BoxSub<FT>& sub) {
        mBag.insert(sub);
    }

    /**
     * Returns and removes one subproblem from the bag
     * @return box
     */
    BoxSub<FT> get() {
        typename QType::iterator i;
        if (mDirection == Directions::TAKE_UPPER) {
            i = mBag.end();
            i--;
        } else {
            i = mBag.begin();
        }
        BoxSub<FT> b = *i;
        mBag.erase(i);
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
        typename QType::iterator i = mBag.begin();
        std::ostringstream os;
        for(i = mBag.begin(); i != mBag.end(); i ++ ) {
            if(i != mBag.begin())
                os << ", ";
            os << i->toString();            
        }
        return os.str();
    }

    /**
     * Setup direction of pop up in a queue
     * @param dir direction
     */
    void setDirection(int dir) {
        mDirection = dir;
    }

private:

    QType mBag;

    int mDirection;

};


#endif	/* BOXSUBPOOL_HPP */

