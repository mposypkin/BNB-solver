/* 
 * File:   boxconproblem.hpp
 * Author: mikdebian
 *
 * Created on April 16, 2013, 4:51 AM
 * Box constrained problem
 */

#ifndef BOXCONPROBLEM_HPP
#define	BOXCONPROBLEM_HPP

#include <problems/optlib/objective.hpp>
#include <util/box/box.hpp>
/**
 * Class for the problem definition for boxed constraint problem
 */
template <typename FT> struct BoxconProblem {
    /**
     * Box constraint
     */
    Box<FT> mBox;
    
    /**
     * The objective
     */
    Objective<FT>* mObj;
};


#endif	/* BOXCONPROBLEM_HPP */

