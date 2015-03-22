/* 
 * File:   hsgreedy.hpp
 * Author: posypkin
 * Heuristic for knapsack: guessing first n units in a tuple
 *
 * Created on December 11, 2012, 09:28 AM
 */

#ifndef HSNTUPLE_HPP
#define	HSNTUPLE_HPP

#include <problems/onedknap/common/knapdata.hpp>
#include "hsheur.hpp"
#include "hsutils.hpp"

template <class T> class HSNTuple : public HSHeur<T> {
public:

    /**
     * Constructor
     * @param kd knapsack data
     * @param n length of the tuple to guess
     */
    HSNTuple(const KnapData<T>* kd, int n) {
        mKD = kd;
        mX.alloc(mKD->mN);
        mTL = n;
    }

    bool search(const HSSub<T>& sub, HSSol<T> &sol, long long& nsteps) {
        T pold = sub.mP;
        T wold = sub.mW;
        bool improved = false;
        int I, E;
        if (sub.mW > mKD->mC)
            return false;
        if (sub.mX[sub.mH] == 2)
            I = sub.mH;
        else
            I = sub.mH + 1;

        if (I > mKD->mN - mTL - 1)
            return false;

        for (int i = 0; i < mTL; i++) {
            mX[I + i] = 1;
            wold += mKD->mCoe[i].mW;
            pold += mKD->mCoe[i].mP;
        }
        E = I + mTL - 1;
        T w = wold, p = pold;
        do {
            if (w <= mKD->mC) {
                wold = w;
                pold = p;
                for (int i = E + 1; i < mKD->mN; i++) {
                    if (w + mKD->mCoe[i].mW <= mKD->mC) {
                        mX[i] = 1;
                        w += mKD->mCoe[i].mW;
                        p += mKD->mCoe[i].mP;
                    } else
                        mX[i] = 0;
                }
#if 0
                printf("===========\n");
                for (int k = I; k <= E; k++) {
                    printf("%d", mX[k]);
                }
                printf("\nw = %d, p = %d\n", w, p);
#endif
                if (p > sol.mP) {
                    printf("IMPROVED from %d to %d\n", sol.mP, p);
                    int i;
                    improved = true;
                    for (i = 0; i < sub.mH; i++)
                        sol.mX[i] = sub.mX[i] % 2;
                    for (; i < mKD->mN; i++)
                        sol.mX[i] = mX[i];
                    sol.mP = p;
                    printf("sol: %s\n", HSUtils<T>::HSSol2string(sol).c_str());
                }
                w = wold;
                p = pold;
            }
        } while (shift(E, w, p));
#if 0
        printf("<<====\n");
        printf("sub: %s\n", HSUtils<T>::HSSub2string(sub).c_str());
        printf("sol: %s\n", HSUtils<T>::HSSol2string(sol).c_str());
        printf("====>>\n");
#endif        
        printf("Finished NTUPLE\n");
        return improved;
    }

private:

    bool shift(int& e, T& w, T& p) {
        if (e <= (mKD->mN - 2)) {
            mX[e] = 0;
            w -= mKD->mCoe[e].mW;
            p -= mKD->mCoe[e].mP;
            e++;
            mX[e] = 1;
            w += mKD->mCoe[e].mW;
            p += mKD->mCoe[e].mP;
            return true;
        } else {
            int i = e;
            int j = 0;
            bool z = false;
            while (true) {
                //printf("O: j = %d, i = %d\n",j,i);
                if (j == mTL) {
                    return false;
                }
                if (mX[i] == 1) {
                    w -= mKD->mCoe[i].mW;
                    p -= mKD->mCoe[i].mP;
                    mX[i] = 0;
                    if (z) {
                        for (int k = 0; k <= j; k++) {
                            int l = i + k + 1;
                            mX[l] = 1;
                            w += mKD->mCoe[l].mW;
                            p += mKD->mCoe[l].mP;
                        }
                        e = i + j + 1;
                        return true;
                    } else {
                        i--;
                        j++;

                    }
                } else {
                    i--;
                    z = true;
                }
            }
        }
    }
    const KnapData<T>* mKD;
    SmartArrayPtr<T> mX;
    int mTL;

};



#endif	/* HSNTUPLE_HPP */

