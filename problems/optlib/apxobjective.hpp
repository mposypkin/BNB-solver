/* 
 * File:   gradapxobjective.hpp
 * Author: posypkin
 *
 * Created on February 2, 2011, 5:43 PM
 */

#ifndef APXOBJECTIVE_HPP
#define	APXOBJECTIVE_HPP

#include "objective.hpp"
#include "optcommon.hpp"
/**
 * Objective that approximates gradient
 */

template <class FT> class ApxObjective : public Objective <FT>
{

public:

    ApxObjective() : Objective<FT>()
    {
        mH = 0.0000001;
    }
    /**
     * Evaluates approximate gradient
     * @param x point
     * @param g gradient
     */
    virtual void grad(const FT* x, FT* g)
    {
        FT xn[OPT_MAX_PARAMETERS];
        int n = Objective<FT>::getDim();
        for(int i = 0; i < n; i ++) {
            xn[i] = x[i];
        }
        for(int i = 0; i < n; i ++) {
            xn[i] = x[i] + mH;
            FT ff = this->func(xn);
            xn[i] = x[i] - mH;
            FT fb = this->func(xn);
            g[i] = (ff - fb) / (2. * mH);
        }
    }

    /**
     * Set approximation step
     * @param h
     */
    void setH(FT h)
    {
        mH = h;
    }
private:
    FT mH;
};


#endif	/* APXOBJECTIVE_HPP */

