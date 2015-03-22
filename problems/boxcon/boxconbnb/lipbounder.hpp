/* 
 * File:   lipbounder.hpp
 * Author: mikdebian
 *
 * Created on April 15, 2013, 8:17 AM
 * 
 * Simple Lipschitzian Bounder
 */

#ifndef LIPBOUNDER_HPP
#define	LIPBOUNDER_HPP

#include <problems/optlib/objective.hpp>
#include <util/box/boxutils.hpp>
#include <util/common/vec.hpp>

#include "boxconbnb.hpp"



template <typename FT> class LipBounder : public BoxconBNB<FT>::Bounder {
public:
    
    typedef BoxconBNB<FT> BOXCON;

    /**
     * Constructor
     * @param obj objective function
     */
    LipBounder(Objective<FT>* obj) : mObj(obj), mLC(0) {
    }
    
    /**
     * Setup Lipschitz constant
     * @param lc Lipschitz constant value
     */
    void setLipConst(FT lc) {
        mLC = lc;
    }

    void bound(typename BOXCON::Sub& sub, typename BOXCON::Solution& incumbent) {
        int n = sub.mBox.mDim;
        mX.realloc(n);        
        for (int i = 0; i < n; i++)
            mX[i] = 0.5 * (sub.mBox.mA[i] + sub.mBox.mB[i]);
        FT fv = mObj->func((FT*)mX);
        if(fv < incumbent.mValue) {
            incumbent.mValue = fv;
            if(incumbent.mX.isNil())
                incumbent.mX.alloc(n);
            VecUtils::vecCopy(n, (FT*)mX, (FT*)(incumbent.mX));            
        }
        FT r = BoxUtils::radius(sub.mBox);
        sub.mBound = fv - mLC * r;
    }

private:
    FT mLC;
    Objective <FT>* mObj;
    SmartArrayPtr<FT> mX;

};


#endif	/* LIPBOUNDER_HPP */

