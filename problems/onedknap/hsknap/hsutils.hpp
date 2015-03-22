/* 
 * File:   hsutils.hpp
 * Author: posypkin
 *
 * Utilities to work with HS BnB classes
 *
 * Created on August 25, 2012, 10:41 AM
 */

#ifndef HSUTILS_HPP
#define	HSUTILS_HPP

#include <string>
#include <sstream>
#include <stdlib.h>
#include "problems/onedknap/common/knapdata.hpp"
#include "hssub.hpp"
#include "hssol.hpp"

/**
 * Various auxilary utilities to work with HS BnB algorithm
 */
template <class T>class HSUtils {
public:

    /**
     * Produces string representation for a subproblem
     * @param sub subproblem
     * @return resulting string
     */
    static std::string HSSub2string(const HSSub<T> &sub) {
        std::ostringstream os;
        os << "fro: " << sub.mFrozen << ", h: " << sub.mH << ", p: " << sub.mP << ", w: " << sub.mW << ", ";
        os << "fw: " << sub.mFW << ", fp: " << sub.mFP << ", ";
        for (int i = 0; i < sub.mX.size(); i++) {
            os << sub.mX[i];
        }
        return os.str();
    }

    /**
     * Produces string representation for a solution
     * @param sol solution
     * @return resulting string
     */
    static std::string HSSol2string(const HSSol<T> &sol) {
        std::ostringstream os;
        os << sol.mP << ", ";
        for (int i = 0; i < sol.mX.size(); i++) {
            os << sol.mX[i];
        }
        return os.str();
    }

    /**
     * Generates the intial subproblem to start search
     * @param kd knapsack data
     * @return initial subproblem
     */
    static HSSub<T> initialSubproblem(const KnapData<T> &kd) {
        HSSub<T> sub;
        sub.mX.alloc(kd.mN);
        sub.mFrozen = -1;
        sub.mH = 0;
        sub.mP = 0;
        sub.mW = 0;
        sub.mFW = kd.mTW;
        sub.mFP = kd.mTP;
        for (int i = 0; i < kd.mN; i++) {
            sub.mX[i] = 2;
        }
        return sub;
    }

    static HSSol<T> emptySolution(const KnapData<T>& kd) {
        HSSol<T> sol;
        sol.mX.alloc(kd.mN);
        sol.mP = 0;
        for (int i = 0; i < kd.mN; i++) {
            sol.mX[i] = 0;
        }
        return sol;
    }

    static HSSol<T> reorderSolution(const KnapData<T>& kd, const HSSol<T>& osol) {
        HSSol<T> sol;
        sol.mX.alloc(kd.mN);
        sol.mP = osol.mP;
        for(int i = 0; i < kd.mN; i ++) {
            sol.mX[kd.mCoe[i].mI] = osol.mX[i];
        }
        return sol;
    }
 };



#endif	/* HSUTILS_HPP */

