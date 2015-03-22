/* 
 * File:   mtbounder.hpp
 * Author: posypkin
 *
 * The bounder to evluate the MT bound
 *
 */

#ifndef MTBOUNDER_HPP
#define	MTBOUNDER_HPP

#include <util/common/utilmacro.hpp>
#include "hsbounder.hpp"
#include "hsutils.hpp"

/**
 * Linear relaxation bound
 */
template <class T> class MTBounder : public HSBounder <T> {
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
        if (s < (kd.mN - 1)) {
            double e = kd.mCoe[s + 1].mE;
            T ub1 = ub + (T)((kd.mC - w) * e);
            e =  kd.mCoe[s - 1].mE;
            T ub2 = (T)((double)ub + (double)kd.mCoe[s].mP + (kd.mC - w - kd.mCoe[s].mW) * e);
#if 0
            e = (double) kd.mCoe[2 * s] / (double) kd.mCoe[2 * s + 1];
            T ubl = ub + (T) ((kd.mC - w) * e);
#endif
            ub = BNBMAX(ub1, ub2);
#if 0
            if(ub < ubl) {
                printf("record = %d\n", record.mP);
                printf("sub: %s\n", HSUtils<T>::HSSub2string(sub).c_str());
                printf("ub = %d, ubl = %d, ub1 = %d, ub2 = %d\n", ub, ubl, ub1, ub2);
            }
#endif
        } else if (s < kd.mN) {
            ub += (T) ((kd.mC - w) * kd.mCoe[s].mE);
        }
        //printf("ub = %lf\n", ub);
        if (ub > record.mP)
            return false;
        else
            return true;
    }
};


#endif	/* MTBOUNDER_HPP */

