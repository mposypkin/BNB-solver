/* 
 * File:   bncsub.hpp
 * Author: medved
 *
 * Created on January 10, 2015, 6:00 PM
 */

#ifndef BNCSUB_HPP
#define	BNCSUB_HPP

#include <vector>
#include <util/box/box.hpp>
#include <problems/nlp/cuts/cut.hpp>
#include <problems/nlp/cuts/recstore.hpp>

/**
 * Subproblem for branch-and-cut method
 */
template <class FT> struct BNCSub {
    /**
     * Cuts for this subproblem
     */
    std::vector< Cut<FT> > mCuts;

    /**
     * Subproblem's box
     */
    Box<FT> mBox;
       
 };

#endif	/* BNCSUB_HPP */

