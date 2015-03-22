/* 
 * File:   compbounder.hpp
 * Author: posypkin
 *
 * Created on July 23, 2012, 10:52 AM
 *
 * Composite bounder
 *
 */

#ifndef COMPBOUNDER_HPP
#define	COMPBOUNDER_HPP

#include <vector>
#include "bounder.hpp"

 template <typename FT> class CompositeBounder : public Bounder <FT> {
 public:
    /**
     * Retrieve the lower bounds
     *
     * @param box the box to evaluate the bound
     * @param crit the place to store the lower bounds for criteria (NULL means no bound is needed)
     * @param cons the place to store the lower bounds for constraints (NULL means no bound is needed)
     */
    void getLowerBound(const Box<FT>& box, FT* crit, FT* cons) const {
        typename std::vector< Bounder<FT>* >::const_iterator i;
        for(i = mBounders.begin(); i != mBounders.end(); i ++) {
            (*i)->getLowerBound(box, crit, cons);
        }
    }


    /**
     * Retrieve the upper bound
     *
     * @param box the box to evaluate the bound
     * @param crit the place to store the upper bounds for criteria (NULL means no bound is needed)
     * @param cons the place to store the upper bounds for constraints (NULL means no bound is needed)
     */
    void getUpperBound(const Box<FT>& box, FT* crit, FT* cons) const {
        typename std::vector< Bounder<FT>* >::const_iterator i;
        for(i = mBounders.begin(); i != mBounders.end(); i ++) {
            (*i)->getUpperBound(box, crit, cons);
        }
    }

    /**
     * Pushed new bounder to the composite bounder
     *
     * @param bounder new bounder
     */
    void push(Bounder<FT>* bounder) {
        mBounders.push_back(bounder);
    }

 private:
     std::vector < Bounder<FT> * > mBounders;
 };

#endif	/* COMPBOUNDER_HPP */

