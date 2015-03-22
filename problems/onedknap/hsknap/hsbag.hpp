/* 
 * File:   hsbag.hpp
 * Author: posypkin
 *
 * "Bag" to store the subproblems for Hrowitz-Sahni method
 * Created on September 12, 2012, 7:02 PM
 */

#ifndef HSBAG_HPP
#define	HSBAG_HPP

#include <set>

#include "hssub.hpp"
#include "hscompare.hpp"
#include "hsutils.hpp"

template <class T> class HSBag {
public:
    typedef std::multiset< HSSub<T>, HSCompare<T> > QType;

    /**
     * Put subproblem to the bag
     * @param sub subproblem to put
     */
    void put(HSSub<T> &sub) {
        mBag.insert(sub);
    }

    /**
     * Returns and removes one subproblem from the bag
     * @return the subproblem
     */
    HSSub<T> get() {
        typename QType::iterator i;
        i = mBag.begin();
        HSSub<T> b = *i;
        mBag.erase(i);
        return b;
    }

    /**
     * Returns the reference to the topmost subproblem
     * but does not remove it
     * @return reference to the topmost subproblem
     */
    const HSSub<T>& top() {
        typename QType::const_iterator i = mBag.begin();
        return *i;
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
        for (i = mBag.begin(); i != mBag.end(); i++) {
            if (i != mBag.begin())
                os << ", ";
            os << HSUtils<T>::HSSub2string(*i);
        }
        return os.str();
    }

    /**
     * Clears the bag
     */
    void clear() {
        mBag.clear();
    }

private:

    QType mBag;
};


#endif	/* HSBAG_HPP */

