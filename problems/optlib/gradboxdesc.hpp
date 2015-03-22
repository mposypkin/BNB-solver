/* 
 * File:   gradboxdesc.hpp
 * Author: posypkin
 *
 * Gradient descent method with box constraints
 * Created on April 23, 2011, 8:35 AM
 */

#ifndef GRADBOXDESC_HPP
#define	GRADBOXDESC_HPP

#include "locboxoptim.hpp"
#include "util/common/vec.hpp"
#include "util/common/bnberrcheck.hpp"

template <typename FT> class GradBoxDescent : public LocalBoxOptimizer <FT> {
public:

    /**
     * Determines stopping conditions
     */
    class Stopper {
    public:
        /**
         * Returns true when the search should stop
         * @param xdiff difference between old and new x
         * @param fdiff difference between old and new f value
         * @param gnorm gradient norm
         * @param fval function value
         * @param n current step number 
         */
        virtual bool stopnow(FT xdiff, FT fdiff, FT gnorm, FT fval, int n) = 0;
    };
    
    /**
     * The constructor
     *
     * @param box bounding box
     */
    GradBoxDescent(Box<FT>& box,  Stopper* stopper) : 
      LocalBoxOptimizer <FT> (box),
      mStopper(stopper)
    {
    }

    /**
     * Setup objective
     * @param obj objective function
     */
    void setObjective(Objective<FT>* obj) {
        LocalOptimizer<FT>::setObjective(obj);
        mG.realloc(obj->getDim());
        mXk.realloc(obj->getDim());
    }


    /**
     * Perform search
     * @param x start point and result
     * @param v pointer to the resulting value
     * @return true if search converged and false otherwise
     */
    bool search(FT* x, FT* v) {
        bool rv;
        BNB_ASSERT(mStopper);
        Objective<FT>* obj = LocalOptimizer<FT>::getObjective();

        project(x);
        FT uold = obj->func(x);
        FT u;
        FT g = 0.01;
        int i = 0;
        int n = obj->getDim();
        for (;;) {
            i++;
            obj->grad(x, mG);
            double gnorm = VecUtils::vecNormTwo(n, (double*)mG);
            VecUtils::vecSaxpy(n, x, (FT*) mG, -g, (FT*) mXk);
            project(mXk);
            u = obj->func(mXk);
            FT xdiff = VecUtils::vecDist(n, x, (FT*) mXk);
            FT fdiff = BNBABS(u - uold);


            if (mStopper->stopnow(xdiff, fdiff, gnorm, uold, i)) {
                *v = u;
                rv = true;
                break;
            }
            if (u >= uold) {
                g *= 0.5;
            } else {
                g *= 1.75;
                VecUtils::vecCopy(n, (FT*) mXk, x);
                uold = u;
            }
        }
        return true;
    }

private:

    void project(FT* x) {
        int n = LocalOptimizer<FT>::getObjective()->getDim();
        for (int i = 0; i < n; i++) {
            if (x[i] < LocalBoxOptimizer<FT>::mBox.mA[i])
                x[i] = LocalBoxOptimizer<FT>::mBox.mA[i];
            else if (x[i] > LocalBoxOptimizer<FT>::mBox.mB[i])
                x[i] = LocalBoxOptimizer<FT>::mBox.mB[i];
        }
    }


    Stopper *mStopper;
    SmartArrayPtr<FT> mG;
    SmartArrayPtr<FT> mXk;
};


#endif	/* GRADBOXDESC_HPP */

