/* 
 * File:   numgradobjective.hpp
 * Author: medved
 *
 * Created on October 26, 2015, 12:20 PM
 */

#ifndef NUMGRADOBJECTIVE_HPP
#define	NUMGRADOBJECTIVE_HPP

#include <util/common/bnbdebug.hpp>

#include "objective.hpp"
#include "optcommon.hpp"

/**
 * Objective wrapper that numerically approximates gradients with finite differences
 */
template <class FT> class NumGradObjective : public Objective <FT> {
public:

    NumGradObjective(Objective <FT>& obj) : mObj(obj) {
        Objective<FT>::setDim(obj.getDim());
        Objective<FT>::setKind(obj.getKind());
    }

    FT func(const FT* x) {
        return mObj.func(x);
    }

    /**
     * Evaluates approximate gradient
     * @param x point
     * @param g gradient
     */
    virtual void grad(const FT* x, FT* g) {
        FT xn[OPT_MAX_PARAMETERS];
        int n = Objective<FT>::getDim();
        for (int i = 0; i < n; i++) {
            xn[i] = x[i];
        }
        for (int i = 0; i < n; i++) {
            xn[i] = x[i] + mH;
            FT ff = this->func(xn);
            xn[i] = x[i] - mH;
            FT fb = this->func(xn);
            g[i] = (ff - fb) / (2. * mH);
#if 0            
            if (BNBABS(g[i]) > 1E+7) {
                ::bnbDebugVar0 = 1;
                std::cout.precision(10);
                std::cout << "h = " << mH << "\n";
                std::cout << "Forward:\n";
                xn[i] = x[i] + mH;
                ff = this->func(xn); 
                VecUtils::vecPrint(n, xn);
                std::cout << "Backward:\n";
                xn[i] = x[i] - mH;
                fb = this->func(xn);
                VecUtils::vecPrint(n, xn);
                
                // TMP 
                std::cout << "i = " << i << "ff = " << ff << ", " << "fb = " << fb << ", gi = " << g[i] << "\n";
                // TMP
                ::bnbDebugVar0 = 0;
                //exit(-1);
            }
#endif            
        }
    }

    /**
     * Set approximation step
     * @param h
     */
    void setH(FT h) {
        mH = h;
    }

private:
    FT mH = 1E-6;
    Objective<FT>& mObj;
};

#endif	/* NUMGRADOBJECTIVE_HPP */

