/* 
 * File:   boxconproblem.hpp
 * Author: mikdebian
 *
 * Created on August 16, 2014, 4:51 AM
 * Generic Non-linear programming problem statement
 */

#ifndef NLPPROBLEM_HPP
#define	NLPPROBLEM_HPP

#include <vector>
#include <problems/optlib/objective.hpp>
#include <util/box/box.hpp>
#include "constraint.hpp"

#define BOXCON_MAX_VARS 10 * 1024


/**
 * Class for the problem definition for non-linear programming
 */
template <typename FT> struct NlpProblem {
    /**
     * Types of variables
     */
    struct VariableTypes {
        /**
         * Normal continuos variable
         */
        static const unsigned int GENERIC = 0;
        
        /**
         * Integral
         */
        static const unsigned int INTEGRAL = 1;
        
        /**
         * Boolean
         */
        static const unsigned int BOOLEAN = 2;        
    };
    
    /**
     * Box constraint (bounding box)
     * for integral and boolean variables respective box bounds should be integral or boolean
     */
    Box<FT> mBox;
    
    /**
     * The objective
     */
    Objective<FT>* mObj;
    
    /**
     * Constraints
     */
    std::vector<Constraint<FT>* > mCons;
    
    /**
     * Characteristics of variables
     */
    std::vector<unsigned int> mVariables;
};


#endif	/* NLPPROBLEM_HPP */

