/* 
 * File:   mgradesc.hpp
 * Author: posypkin
 *
 * Multicriteria analog to the gradient descent method
 *
 * Created on April 25, 2011, 12:21 PM
 */

#ifndef MGRADESC_HPP
#define	MGRADESC_HPP

#include <util/ineq/ineqsolver.hpp>
#include <util/common/vec.hpp>
#include <util/common/util.hpp>

#include "multilocsearch.hpp"

template <typename FT> class MultiGradDescent : public MultiLocalSearch <FT> {
public:

    /**
     * Constructor
     * @param isolver inequality solver
     */
    MultiGradDescent(MultiData<FT> md, IneqSolver<FT>* isolver = NULL) :
    MultiLocalSearch<FT>(md),
    mIneqSolver(isolver),
    mPk(md.mBox.mDim, md.mCriteria.size()) {
        int nv = md.mBox.mDim;
        int nc = md.mCriteria.size();
        int sz = nc * (nv + 1);
        mG.realloc(sz);
        mP.realloc(nv);
        mXtol = 0.001;
        mFtol = 0.001;
    }

    void search(MultiPoint<FT>& mp) {
        FT g = 1;
        BNB_ASSERT(MultiLocalSearch<FT>::mMD.mConstraints.empty());
        int sz = MultiLocalSearch<FT>::mMD.mCriteria.size();
        int nv = MultiLocalSearch<FT>::mMD.mBox.mDim;
        int m = mp.mCrit.mDim;
        for (;;) {
            for (int i = 0; i < sz; i++) {
                MultiLocalSearch<FT>::mMD.mCriteria[i]->grad(mp.mParam.mX, mG + i * (nv + 1));
                mG[i * (nv + 1) + nv] = 0;
            }
            mIneqSolver->setInequalities(nv, sz, mG);
            if (!mIneqSolver->findFeasible(mP))
                break;
            VecUtils::vecSaxpy(nv, (FT*) mp.mParam.mX, (FT*) mP, g, (FT*) mPk.mParam.mX);
            project(mPk.mParam.mX);
            eval(mPk);
            FT e = getMinValue<FT > ();
            for (int i = 0; i < m; i++) {
                e = BNBMAX(e, mPk.mCrit.mX[i] - mp.mCrit.mX[i]);
            }
            //printf("e = %lf\n", e);
            if(e > -mFtol) {
                if(g < mXtol)
                    break;
                g *= 0.5;
            } else {
                copy(mPk, mp);
            }
        }
    }

    void setIneqSolver(IneqSolver<FT>* isolver) {
        mIneqSolver = isolver;
    }

private:

    void project(FT* x) const {
        int n = MultiLocalSearch<FT>::mMD.mBox.mDim;
        for (int i = 0; i < n; i++) {
            if (x[i] < MultiLocalSearch<FT>::mMD.mBox.mA[i])
                x[i] = MultiLocalSearch<FT>::mMD.mBox.mA[i];
            else if (x[i] > MultiLocalSearch<FT>::mMD.mBox.mB[i])
                x[i] = MultiLocalSearch<FT>::mMD.mBox.mB[i];
        }
    }

    void eval(MultiPoint<FT>& mp) const {
        int m = mp.mCrit.mDim;
        for (int i = 0; i < m; i++) {
            mp.mCrit.mX[i] = MultiLocalSearch<FT>::mMD.mCriteria[i]->func(mp.mParam.mX);
        }
    }


    void copy(MultiPoint<FT>& mps, MultiPoint<FT>& mpt) const {
        int pd = mps.mParam.mDim;
        int cd = mps.mCrit.mDim;
        for(int i = 0; i < pd; i ++) {
            mpt.mParam.mX[i] = mps.mParam.mX[i];
        }
        for(int i = 0; i < cd; i ++) {
            mpt.mCrit.mX[i] = mps.mCrit.mX[i];
        }
    }

    IneqSolver<FT> *mIneqSolver;
    SmartArrayPtr<FT> mG;
    SmartArrayPtr<FT> mP;
    MultiPoint<FT> mPk;
    FT mXtol;
    FT mFtol;

};



#endif	/* MGRADESC_HPP */

