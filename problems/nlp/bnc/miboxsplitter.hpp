/* 
 * File:   miboxsplitter.hpp
 * Author: medved
 *
 * Created on June 8, 2015, 6:51 PM
 */

#ifndef MIBOXSPLITTER_HPP
#define	MIBOXSPLITTER_HPP

#include <math.h>
#include <vector>
#include <util/box/boxutils.hpp>
#include <problems/nlp/common/nlpproblem.hpp>
#include "boxsplitter.hpp"

/**
 * Box splitter for mixed integer and boolean problems
 */
template <class FT> class MIBoxSplitter : public BoxSplitter<FT> {
public:

    /**
     * Constructor
     * @param vars vector of variables characteristics
     */
    MIBoxSplitter(const std::vector<unsigned int>& vars) : mVars (vars) {
        
    }

    void split(const Box<FT>& box, std::vector< Box<FT> >& boxv) {
        int n = box.mDim;
        Box<FT> b1(n), b2(n);
        BoxUtils::copy(box, b1);
        BoxUtils::copy(box, b2);

        /* Longest edge */
        FT lmax = 0;
        int ind_lmax = -1;
        /* Longest integral edge */
        FT limax = 0;
        int ind_limax = -1;
        /* Longest boolean edge */
        FT lbmax = 0;
        int ind_lbmax = -1;

        for (int i = 0; i < n; i++) {
            FT l = box.mB[i] - box.mA[i];
            if (l > lmax) {
                lmax = l;
                ind_lmax = i;
            }
            if (mVars[i] == NlpProblem<FT>::VariableTypes::INTEGRAL) {
                if (l > limax) {
                    limax = l;
                    ind_limax = i;
                }
            }
            if (mVars[i] == NlpProblem<FT>::VariableTypes::BOOLEAN) {
                if (l > lbmax) {
                    lbmax = l;
                    ind_lbmax = i;
                }
            }
        }
        if (ind_lmax != -1) { /* Box is not a point */
            if (ind_lbmax != -1) {
                b1.mB[ind_lbmax] = 0;
                b2.mA[ind_lbmax] = 1;
            } else if (ind_limax != -1) {
                FT c = 0.5 * (box.mA[ind_limax] + box.mB[ind_limax]);
                b1.mB[ind_limax] = floor(c);
                b2.mA[ind_limax] = ceil(c);
            } else {
                FT c = 0.5 * (box.mA[ind_lmax] + box.mB[ind_lmax]);
                b1.mB[ind_lmax] = c;
                b2.mA[ind_lmax] = c;
            }
            boxv.push_back(b1);
            boxv.push_back(b2);
        }
    }

private:

    const std::vector<unsigned int>& mVars;
};


#endif	/* MIBOXSPLITTER_HPP */

