/* 
 * File:   bounder.hpp
 * Author: posypkin
 *
 * Created on April 29, 2011, 12:43 AM
 */

#ifndef BOUNDER_HPP
#define	BOUNDER_HPP

#include <util/common/bnberrcheck.hpp>
#include <util/box/box.hpp>
#include <problems/pareto/multiopt/point.hpp>
/**
 * Evaluate lower and upper bound for the objectives
 */
template <typename FT> class Bounder {
public:
    
    /**
     * Retrieve the lower bounds
     *
     * @param box the box to evaluate the bound
     * @param crit the place to store the lower bounds for criteria (NULL means no bound is needed)
     * @param cons the place to store the lower bounds for constraints (NULL means no bound is needed)
     */
    virtual void getLowerBound(const Box<FT>& box, FT* crit, FT* cons) const {
        BNB_ERROR_REPORT("Lower bound not implemented");
    }
    
    
    /**
     * Retrieve the upper bound
     *
     * @param box the box to evaluate the bound
     * @param crit the place to store the upper bounds for criteria (NULL means no bound is needed)
     * @param cons the place to store the upper bounds for constraints (NULL means no bound is needed)
     */
    virtual void getUpperBound(const Box<FT>& box, FT* crit, FT* cons) const {
        BNB_ERROR_REPORT("Upper bound not implemented");
    }
};



#endif	/* BOUNDER_HPP */

