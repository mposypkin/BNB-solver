/* 
 * File:   gradboxdesc.hpp
 * Author: posypkin
 *
 * Gradient descent method with box constraints
 * Created on April 23, 2011, 8:35 AM
 */

#ifndef GRADBOXDESC_HPP
#define	GRADBOXDESC_HPP

#include <limits>
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
         * @param x point
         * @param grad gradient 
         * @param fval function value
         * @param n current step number 
         */
        virtual bool stopnow(FT xdiff, FT fdiff, FT* x, FT* grad, FT fval, int n) = 0;
    };

    /**
     * Options for Gradient Box Descent method
     */
    struct Options {
        /**
         * Initial value of granularity
         */
        FT mGInit = 0.01;

        /**
         * Increase in the case of success
         */
        FT mInc = 1.75;
        /**
         * Decrease in the case of failur
         */
        FT mDec = 0.5;
    };

    /**
     * The constructor
     *
     * @param box bounding box
     */
    GradBoxDescent(Box<FT>& box, Stopper* stopper) :
    LocalBoxOptimizer <FT> (box),
    mStopper(stopper) {
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
        FT g = mOptions.mGInit;
        int i = 0;
        int n = obj->getDim();
        FT xdiff = std::numeric_limits<FT>::max();
        FT fdiff = std::numeric_limits<FT>::max();

        for (;;) {
            i++;

            obj->grad(x, mG);
            if (mStopper->stopnow(xdiff, fdiff, (FT*) x, (FT*) mG, uold, i)) {
                *v = uold;
                rv = true;
                break;
            }
            VecUtils::vecSaxpy(n, x, (FT*) mG, -g, (FT*) mXk);
            project(mXk);
            u = obj->func(mXk);

            if (u >= uold) {
                g *= mOptions.mDec;
            } else {
                g *= mOptions.mInc;
                VecUtils::vecCopy(n, (FT*) mXk, x);
                uold = u;
                xdiff = VecUtils::vecDist(n, x, (FT*) mXk);
                fdiff = BNBABS(u - uold);
            }



        }
        return true;
    }

    /**
     * Retrieve options
     * @return options
     */
    Options& getOptions() {
        return mOptions;
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
    Options mOptions;
};


#endif	/* GRADBOXDESC_HPP */

