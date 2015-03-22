/* 
 * File:   tnkbounder.hpp
 * Author: posypkin
 *
 * Bounder for TNK test function
 *
 * Created on January 30, 2013, 7:39 PM
 */

#ifndef TNKBOUNDER_HPP
#define	TNKBOUNDER_HPP

#include <math.h>
#include <problems/pareto/multicov/bounder.hpp>
#include <util/common/util.hpp>
#include <util/common/interval.hpp>

class TNKBounder : public Bounder <double> {
public:

    /**
     * Retrieve the lower bounds
     *
     * @param box the box to evaluate the bound
     * @param crit the place to store the lower bounds for criteria (NULL means no bound is needed)
     * @param cons the place to store the lower bounds for constraints (NULL means no bound is needed)
     */
    void getLowerBound(const Box<double>& box, double* crit, double* cons) const {
        crit[0] = box.mA[0];
        crit[1] = box.mA[1];
        double l, u, l1, u1, c = 1;
        l = box.mA[0] / BNBMAX(box.mB[1], MINX);
        u = box.mB[0] / BNBMAX(box.mA[1], MINX);
        l = 16 * atan(l);
        u = 16 * atan(u);
        BNBInterval<double>::cos(l, u, &l1, &u1);
        l = 0.1 * l1;
        u = 0.1 * u1;
        c += l;
        c -= box.mB[0] * box.mB[0];
        c -= box.mB[1] * box.mB[1];
        cons[0] = c;
        c = - 0.5;
        l = box.mA[0] - 0.5;
        u = box.mB[0] - 0.5;
        BNBInterval<double>::sqr(l, u, &l1, &u1);
        c += l1;
        l = box.mA[1] - 0.5;
        u = box.mB[1] - 0.5;
        BNBInterval<double>::sqr(l, u, &l1, &u1);
        c += l1;
        cons[1] = c;
    }


    /**
     * Retrieve the upper bound
     *
     * @param box the box to evaluate the bound
     * @param crit the place to store the upper bounds for criteria (NULL means no bound is needed)
     * @param cons the place to store the upper bounds for constraints (NULL means no bound is needed)
     */
    virtual void getUpperBound(const Box<double>& box, double* crit, double* cons) const {
        BNB_ERROR_REPORT("Upper bound not implemented");
    }
};



#endif	/* TNKBOUNDER_HPP */

