/* 
 * File:   polybounder.hpp
 * Author: posypkin
 *
 * Created on January 19, 2013, 1:08 AM
 */

#ifndef POLYBOUNDER_HPP
#define	POLYBOUNDER_HPP

#include <limits>
#include <vector>
#include <problems/pareto/multiopt/multidata.hpp>
#include <util/poly/polynom.hpp>
#include <util/box/boxutils.hpp>
#include "bounder.hpp"

/**
 * Interval bounder for polynomials
 */
template <typename FT> class PolyBounder : public Bounder<FT> {
public:

    /**
     * Constructor
     * @param crit vector of criteria
     * @param cons vector of constraints
     */
    PolyBounder(const std::vector< Polynom<FT>* > crit, const std::vector< Polynom<FT>* > cons) :
    mCrit(crit),
    mCons(cons) {
    }

    void getLowerBound(const Box<FT>& box, FT* crit, FT* cons) const {
        getBound(box, crit, cons, -1);
    }

    void getUpperBound(const Box<FT>& box, FT* crit, FT* cons) const {
        getBound(box, crit, cons, 1);
    }


private:

    void getBound(const Box<FT>& box, FT* crit, FT* cons, int ul) const {
        int no = mCrit.size();
        FT dummy;
        for (int i = 0; i < no; i++) {
            if (ul == -1)
                mCrit[i]->getBounds(box.mA, box.mB, crit + i, &dummy);
            else
                mCrit[i]->getBounds(box.mA, box.mB, &dummy, crit + i);
        }
        int nc = mCons.size();
        for (int i = 0; i < nc; i++) {
            if (ul == -1)
                mCons[i]->getBounds(box.mA, box.mB, cons + i, &dummy);
            else
                mCons[i]->getBounds(box.mA, box.mB, &dummy, cons + i);
        }
    }


    const std::vector< Polynom<FT>* > mCrit;
    const std::vector< Polynom<FT>* > mCons;
};

#endif	/* LIPBOUNDER_HPP */

