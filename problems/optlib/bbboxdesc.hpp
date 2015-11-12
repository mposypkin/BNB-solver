/* 
 * File:   bbboxdesc.hpp
 * Author: medved
 *
 * Created on November 3, 2015, 5:05 PM
 */

#ifndef BBBOXDESC_HPP
#define	BBBOXDESC_HPP

#include "locboxoptim.hpp"
#include "util/common/vec.hpp"
#include "util/common/bnberrcheck.hpp"

/**
 * Modified gradient descent for Black Box optimization
 */
template <typename FT> class BBBoxDescent : public LocalBoxOptimizer <FT> {
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
         * @param gmin gradient minimal component
         * @param fval function value
         * @param n current step number 
         */
        virtual bool stopnow(FT xdiff, FT fdiff, FT gmin, FT fval, int n) = 0;
    };

    /**
     * Options for Gradient Box Descent method
     */
    struct Options {
        /**
         * Initial value of granularity
         */
        FT mHInit = 0.01;

        /**
         * Increase in the case of success
         */
        FT mInc = 1.75;
        /**
         * Decrease in the case of failure
         */
        FT mDec = 0.5;
        /**
         * Lower bound for granularity
         */
        FT mHLB = 1e-08;
        /**
         * Upper bound on granularity
         */
        FT mHUB = 1e+02;

    };

    /**
     * The constructor
     *
     * @param box bounding box
     */
    BBBoxDescent(Box<FT>& box, Stopper* stopper) :
    LocalBoxOptimizer <FT> (box),
    mStopper(stopper) {
    }

    /**
     * Setup objective
     * @param obj objective function
     */
    void setObjective(Objective<FT>* obj) {
        LocalOptimizer<FT>::setObjective(obj);
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
        int n = obj->getDim();
        FT sft[n];
        FT g[n];
        FT h = mOptions.mHInit;
        FT gmin;
        int imin;
        Box<double> box(LocalBoxOptimizer<FT>::mBox);

        // Computes pseudo gradient
        auto getg = [&] {
            //std::cout << "getg entry\n";
            FT fv = obj->func(x);
            //std::cout << "function computed\n";
            gmin = 0;
            imin = -1;
            for (;;) {
                for (int i = 0; i < n; i++) {
                    FT sf, sb;
                    FT y = x[i] - h;
                    if (y < box.mA[i]) {
                        y = box.mA[i];
                    }
                    sb = y - x[i];
                    FT tmp = x[i];
                    x[i] = y;
                    FT fb = obj->func(x);
                    x[i] = tmp;

                    y = x[i] + h;
                    if (y > box.mB[i]) {
                        y = box.mB[i];
                    }
                    sf = y - x[i];
                    tmp = x[i];
                    x[i] = y;
                    FT ff = obj->func(x);
                    x[i] = tmp;

                    FT db = fb - fv;
                    FT df = ff - fv;
                    if ((df >= 0) && (db >= 0)) {
                        g[i] = 0;
                        sft[i] = 0;
                    } else if (df < db) {
                        sft[i] = sf;
                        g[i] = df;
                    } else {
                        sft[i] = sb;
                        g[i] = db;
                    }
                    if (g[i] < gmin) {
                        imin = i;
                        gmin = g[i];
                    }
                }
                if (imin == -1) {
                    //std::cout << "getg: h = " << h << "\n";
                    if (h > mOptions.mHLB)
                        h *= mOptions.mDec;
                    else break;
                } else
                    break;
            }

        };

        int i = 0;
        FT xk[n];
        int bump = 0;
        for (;;) {
            i++;
            getg();
            if (imin == -1) {
                if(bump --) {
                    std::cout << "BUMP!!!\n";
                    h = mOptions.mHInit;
                    continue;
                }
                rv = true;
                break;
            }
            VecUtils::vecSaxpy(n, x, g, 1. / gmin, xk);
            project(xk);
            FT u = obj->func(xk);
            if (u - uold > gmin) {
                VecUtils::vecCopy(n, x, xk);
                xk[imin] = x[imin] + sft[imin];
                u = uold + gmin;
            }
            FT xdiff = VecUtils::vecDist(n, x, xk);
            FT fdiff = u - uold;
            if (mStopper->stopnow(xdiff, fdiff, gmin, uold, i)) {
                rv = true;
                break;
            }
            if (u >= uold) {
                if (h > mOptions.mHLB)
                    h *= mOptions.mDec;
            } else {
                if (h < mOptions.mHUB)
                    h *= mOptions.mInc;
                VecUtils::vecCopy(n, xk, x);
                uold = u;
            }
        }
        *v = uold;
        return rv;
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
    Options mOptions;
};


#endif	/* BBBOXDESC_HPP */

