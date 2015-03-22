/* 
 * File:   u0bounder.hpp
 * Author: posypkin
 *
 * The bounder to evluate the u0 bound
 *
 * Created on August 27, 2012, 11:34 AM
 */

#ifndef U0BOUNDER_HPP
#define	U0BOUNDER_HPP

#include <iostream>
#include "hsbounder.hpp"

/**
 * Simple U0 bound
 */
template <class T> class U0Bounder : public HSBounder <T> {
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
        if(sub.mW > kd.mC)
            return true;
        if(sub.mP > record.mP) {
            record.mP = sub.mP;
            for(int i = 0; i < kd.mN; i ++)
                record.mX[i] = sub.mX[i] % 2;
        }        
        if( sub.mH >= (kd.mN - 1))
            return true;
        int s = sub.mH + 1;
        T ub = (T)((kd.mC - sub.mW) * kd.mCoe[s].mE + sub.mP);
        printf("ub = %d, sub = %s\n", ub, HSUtils<T>::HSSub2string(sub).c_str());
        if(ub > record.mP)
            return false;
        else
            return true;
    }
};


#endif	/* U0BOUNDER_HPP */

