/* 
 * File:   convobjective.hpp
 * Author: posypkin
 *
 * The optimization objective based on the criteria convolution
 *
 * Created on April 23, 2011, 10:32 PM
 */

#ifndef CONVOBJECTIVE_HPP
#define	CONVOBJECTIVE_HPP

#include <problems/optlib/objective.hpp>
#include <util/common/smartptr.hpp>

#include "multidata.hpp"

template <typename FT> class ConvolObjective : public Objective<FT> {
public:

    /**
     * The Constructor
     * @param criteria the vector of criteria
     * @param coeff the convolution coefficients
     */
    ConvolObjective(std::vector< Objective<FT>* >& criteria, std::vector<FT>& coeff) {
        BNB_ASSERT(mCriteria.size() == mCoeff.size());
        mCriteria = criteria;
        mCoeff = coeff;
        int n = mCriteria[0]->getDim();
        Objective<FT>::setDim(n);
        mAuxg.alloc(n);
    }

    FT func(const FT* x) {
        int m = mCriteria.size();
        FT v = 0;
        for (int i = 0; i < m; i++) {
            v += mCoeff[i] * mCriteria[i]->func(x);
        }
        return v;
    }

    void grad(const FT* x, FT* g) {
        int m = mCriteria.size();
        int n = Objective<FT>::getDim();
        
        for(int j = 0; j < n; j ++)
            g[j] = 0;
        for (int i = 0; i < m; i++) {
            mCriteria[i]->grad(x, mAuxg);
            for(int j = 0; j < n; j ++) {
                g[j] += mCoeff[i] * mAuxg[j];
            }
        }

    }

    /**
     * Get the coefficients vector
     * @return
     */
    std::vector<FT>& getCoeff() {
        return mCoeff;
    }

private:
    std::vector< Objective<FT>* > mCriteria;
    std::vector< FT > mCoeff;
    SmartArrayPtr<FT> mAuxg;

};




#endif	/* CONVOBJECTIVE_HPP */

