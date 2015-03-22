#ifndef __MBFGS_HPP__
#define __MBFGS_HPP__

#include "locoptim.hpp"
#include "cbfgs.h"
#include <util/common/util.hpp>
#include <util/common/smartptr.hpp>

#define DEFAULT_MAX_ITERS 10000

#define DEFAULT_UPDATES 4

#define DEFAULT_EPS 0.001

/**
 * Limited memory BFGS
 */

class MBFGS : public LocalOptimizer <double> {
public:

    MBFGS() {
        mMaxIters = DEFAULT_MAX_ITERS;
        mUpdates = DEFAULT_UPDATES;
        mEps = DEFAULT_EPS;
        mNSteps = 0;
        cbfgsInit();
    }

    /**
     * Perform search
     * @param x start point and result
     * @param v
     * @return true if search converged and false otherwise
     */
    bool search(double* x, double* v) {
        bool rv = true;
        int n;
        int iprint[2] = {-1, 0};
        int fl = 0;
        const double xtol = 2.2204460492503e-16;


        n = LocalOptimizer<double>::mObj->getDim();
        mG.lazyAlloc(n);
        mDiag.lazyAlloc(n);
        mWork.lazyAlloc(n * (2 * mUpdates + 1) + 2 * mUpdates);

        for (int i = 0;; i++) {
            mNSteps++;
            *v = LocalOptimizer<double>::mObj->gradAndFunc(x, mG);
            cbfgs(n, mUpdates, x, *v, mG, 0, mDiag, iprint, mEps, xtol, mWork, &fl, 0.1);
            if (fl == 0)
                break;
            if (i > mMaxIters) {
                rv = false;
                break;
            }
        }
        *v = LocalOptimizer<double>::mObj->func(x);
        return rv;
    }

    /**
     * Install epsilon
     * @param eps 
     */
    void setEps(double eps) {
        mEps = eps;
    }

    /**
     * Install maximal iters counter
     * @param iters iters counter
     */
    void setMaxIters(int iters) {
        mMaxIters = iters;
    }

    /**
     * Setup update number
     */
    void setUpdate(int upd) {
        mUpdates = upd;
    }

    int getSteps() {
        return mNSteps;
    }

    void resetSteps() {
        mNSteps = 0;
    }
    
private:
    SmartArrayPtr<double> mG;
    SmartArrayPtr<double> mDiag;
    SmartArrayPtr<double> mWork;
    int mMaxIters;
    int mUpdates;
    int mNSteps;
    double mEps;
};


#endif
