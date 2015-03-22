/* 
 * File:   multipoint.hpp
 * Author: posypkin
 *
 * Points in multiobjective optimization
 *
 * Created on April 18, 2011, 10:46 PM
 */

#ifndef MULTIPOINT_HPP
#define	MULTIPOINT_HPP

#include <util/common/smartptr.hpp>
#include "point.hpp"

/**
 * Point and respective function values
 */
template <typename FT> struct MultiPoint {

    /**
     * Constructor
     * @param paramd number of parameters
     * @critd number of criteria
     */
    MultiPoint(int paramd, int critd) : mParam(paramd), mCrit(critd) {
    }

    /**
     * Converts the point to
     * @return
     */
    std::string toString() const {
        std::ostringstream os;
        os << "<x>";
        os << mParam.toString();
        os << "</x>";
        os << "<f>";
        os << mCrit.toString();
        os << "</f>";
        return os.str();
    }

    /**
     * Comparison <= operator
     * @param operand multi-point to compare
     * @return the comparison result
     */
    bool operator <=(const MultiPoint & operand) const {
        return (mCrit <= operand.mCrit);
    }

    /**
     * Comparison >= operator
     * @param operand multi-point to compare
     * @return the comparison result
     */
    bool operator >=(const MultiPoint & operand) const {
        return (mCrit >= operand.mCrit);
    }

    /**
     * Parameters
     */
    Point<FT> mParam;

    /**
     * Criteria
     */
    Point<FT> mCrit;
};




#endif	/* MULTIPOINT_HPP */

