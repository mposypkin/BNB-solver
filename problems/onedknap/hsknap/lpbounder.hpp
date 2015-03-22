/* 
 * File:   u0bounder.hpp
 * Author: posypkin
 *
 * The bounder to evluate the u0 bound
 *
 * Created on August 27, 2012, 11:34 AM
 */

#ifndef LPBOUNDER_HPP
#define	LPBOUNDER_HPP

#include "hsbounder.hpp"
#include "hsutils.hpp"

/**
 * Linear relaxation bound
 */
template <class T> class LPBounder : public HSBounder <T> {
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
        T ub = sub.mP;
        T w = sub.mW;
        int s = sub.mH + 1;
        while (s < kd.mN) {
            if (w + kd.mCoe[s].mW > kd.mC)
                break;
            ub += kd.mCoe[s].mP;
            w += kd.mCoe[s].mW;
            s++;
        }
        if (s < kd.mN) {
            ub += (T) ((kd.mC - w) * kd.mCoe[s].mE);
        }
        printf("ub = %d, sub = %s\n", ub, HSUtils<T>::HSSub2string(sub).c_str());
        if (ub > record.mP)
            return false;
        else
            return true;
    }
};


#endif	/* LPBOUNDER_HPP */

