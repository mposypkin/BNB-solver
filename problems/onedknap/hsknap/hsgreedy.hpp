/* 
 * File:   hsgreedy.hpp
 * Author: posypkin
 * Greedy heuristic for knapsack
 *
 * Created on October 7, 2012, 1:50 PM
 */

#ifndef HSGREEDY_HPP
#define	HSGREEDY_HPP

#include <problems/onedknap/common/knapdata.hpp>
#include "hsheur.hpp"
#include "hsutils.hpp"

template <class T> class HSGreedy : public HSHeur<T> {
public:

    HSGreedy(const KnapData<T>* kd) {
        mKD = kd;
        mX.alloc(mKD->mN);
    }

    bool search(const HSSub<T>& sub, HSSol<T> &sol, long long& nsteps) {
        T p = sub.mP;
        T w = sub.mW;
        bool improved = false;
        int I;
        if(sub.mW > mKD->mC)
            return false;
        if(sub.mX[sub.mH] == 2)
            I = sub.mH;
        else
            I = sub.mH + 1;

        for (int i = I; i < mKD->mN; i++) {
            if (w + mKD->mCoe[i].mW <= mKD->mC) {
                mX[i] = 1;
                w += mKD->mCoe[i].mW;
                p += mKD->mCoe[i].mP;
            } else
                mX[i] = 0;
        }
        if (p > sol.mP) {
            printf("IMPROVED from %d to %d\n", sol.mP, p);
            int i;
            improved = true;
            for (i = 0; i < sub.mH; i++)
                sol.mX[i] = sub.mX[i] % 2;
            for (; i < mKD->mN; i++)
                sol.mX[i] = mX[i];
            sol.mP = p;
        }
#if 0
        printf("<<====\n");
        printf("sub: %s\n", HSUtils<T>::HSSub2string(sub).c_str());
        printf("sol: %s\n", HSUtils<T>::HSSol2string(sol).c_str());
        printf("====>>\n");
#endif        
        return improved;
    }

private:

    const KnapData<T>* mKD;
    SmartArrayPtr<T> mX;

};



#endif	/* HSGREEDY_HPP */

