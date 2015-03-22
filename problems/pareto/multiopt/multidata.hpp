/* 
 * File:   multidata.hpp
 * Author: posypkin
 *
 * Source data for constrained multiobjective optimization
 *
 * Created on April 19, 2011, 9:21 PM
 */

#ifndef MULTIDATA_HPP
#define	MULTIDATA_HPP

#include <vector>
#include <problems/optlib/objective.hpp>
#include <util/box/box.hpp>

#include "multipoint.hpp"

/**
 * Source data for constrained multiobjective optimization
 */
template <typename FT> struct MultiData {
    typedef FT DataType;

    /**
     * The constructor
     * @param dim dimension of the parameter space
     */
    MultiData(Box<FT> box) : mBox(box) {

    }

    /**
     * Evaluates criteria and constraints
     * @param x pont
     * @param crit criteria
     * @param cons constraints
     */
    void evaluate(FT* x, FT* crit, FT * cons) {
        int critd = mCriteria.size();
        int consd = mConstraints.size();
        if (crit) {
            for (int i = 0; i < critd; i++) {
                crit[i] = mCriteria[i]->func(x);
            }
        }
        if (cons) {
            for (int i = 0; i < consd; i++) {
                cons[i] = mConstraints[i]->func(x);
            }
        }
    }

    /**
     * Criteria
     */
    std::vector< Objective<FT>* > mCriteria;

    /**
     * Constraints in the form g(x) <= 0
     */
    std::vector< Objective<FT>* > mConstraints;

    /**
     * Bounding box
     */
    Box<FT> mBox;
};


#endif	/* MULTIDATA_HPP */

