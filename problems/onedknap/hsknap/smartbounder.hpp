/* 
 * File:   u0bounder.hpp
 * Author: posypkin
 *
 * The bounder to evluate the u0 bound
 *
 * Created on August 27, 2012, 11:34 AM
 */

#ifndef SMARTBOUNDER_HPP
#define	SMARTBOUNDER_HPP

#include <iostream>
#include "hsbounder.hpp"

/**
 * Smart U0 bound
 */
template <class T> class SmartBounder : public HSBounder <T> {
public:

    /**
     * Checks the bound condition. Assumes that mH points to the currently
     * last filled item!
     * @param kd knapsack data
     * @param sub subproblem
     * @param record incumbent solution
     * @return
     */
    bool BoundAndReduce(const KnapData<T>& kd, const HSSub<T>& sub, HSSol<T>& record) {
        if (sub.mW > kd.mC)
            return true;
        if (sub.mP > record.mP) {
            record.mP = sub.mP;
            for (int i = 0; i < kd.mN; i++)
                record.mX[i] = sub.mX[i] % 2;
        }
        if (sub.mH >= (kd.mN - 1))
            return true;
        T rc = kd.mC - sub.mW;
        if (sub.mFW <= rc) {
            T np = sub.mP + sub.mFP;
            if (np > record.mP) {
                record.mP = np;
                int i = 0;
                for (; i <= sub.mH; i++)
                    record.mX[i] = sub.mX[i];
                for (; i < kd.mN; i++)
                    record.mX[i] = 1;
            }
            return true;
        }
        int s = sub.mH + 1;
        T ub1 = (T) (rc * kd.mCoe[s].mE + sub.mP);
        T ub2;
        if (ub1 > record.mP) {
            T w = sub.mW;
            ub2 = sub.mP;
            while (s < kd.mN) {
                if (w + kd.mCoe[s].mW > kd.mC)
                    break;
                ub2 += kd.mCoe[s].mP;
                w += kd.mCoe[s].mW;
                s++;
            }
            if (s < kd.mN) {
                ub2 += (T) ((kd.mC - w) * kd.mCoe[s].mE);
            }
        } else {
            return true;
        }
        //printf("ub = %d, sub = %s\n", ub, HSUtils<T>::HSSub2string(sub).c_str());
        if (ub2 > record.mP)
            return false;
        else
            return true;
    }
};


#endif	/* SMART_HPP */

