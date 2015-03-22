/* 
 * File:   point.hpp
 * Author: posypkin
 *
 * Class for holding n-dimensional points
 *
 * Created on April 18, 2011, 10:52 PM
 */

#ifndef POINT_HPP
#define	POINT_HPP

#include <string>
#include <sstream>
#include <util/common/smartptr.hpp>

/**
 * Handling points in n-dimensional space
 */
template <typename BT> struct Point {

    /**
     * Constructor
     * @param dim the number of dimensions (space dimension)
     */
    Point(int dim) : mX(dim)
    {
        mDim = dim;
    }

    /**
     * Converts the point to the string
     * @return resulting string
     */
    std::string toString() const {
        std::ostringstream os;        
        for (int i = 0; i < mDim; i++) {
            os << mX[i];
            if(i != mDim - 1)
                os << " ";
        }
        return os.str();
    }

    /**
     * Compares two points <=
     * @param operand right operand
     * @return the result of comparison
     */
    bool operator <=(const Point & operand) const {
        bool rv = true;
        for (int i = 0; i < mDim; i++) {
            if (mX[i] > operand.mX[i]) {
                rv = false;
                break;
            }
        }
        return rv;
    }

   /**
     * Compares two points >=
     * @param operand right operand
     * @return the result of comparison
     */
    bool operator >=(const Point & operand) const {
        bool rv = true;
        for (int i = 0; i < mDim; i++) {
            if (mX[i] < operand.mX[i]) {
                rv = false;
                break;
            }
        }
        return rv;
    }


    /**
     * Number of dimensions
     */
    int mDim;

    /**
     * Holds coordinates
     */
    SmartArrayPtr<BT> mX;

};

#endif	/* POINT_HPP */

