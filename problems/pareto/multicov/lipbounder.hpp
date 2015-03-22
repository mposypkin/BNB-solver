/* 
 * File:   lipbounder.hpp
 * Author: posypkin
 *
 * Created on April 29, 2011, 1:08 AM
 */

#ifndef LIPBOUNDER_HPP
#define	LIPBOUNDER_HPP

#include "bounder.hpp"
#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/cover/common/lipzobjective.hpp>
#include <util/box/boxutils.hpp>
#include <limits>

/**
 * Lipschitzian bounder
 */
template <typename FT> class LipBounder : public Bounder<FT> {
public:

    struct Options {
        /**
         * Basic Lipschitz function
         */
        static const unsigned int LIP_FUNC = 1;

        /**
         * Gradient Lipschitz
         */
        static const unsigned int LIP_GRAD = 1 << 1;
    };

    LipBounder(MultiData<FT>* md) :
    mMD(md),
    mGrad(md->mBox.mDim),
    mBoxCenter(md->mBox.mDim),
    mOptions(Options::LIP_FUNC) {
    }


    void getLowerBound(const Box<FT>& box, FT* crit, FT* cons) const {
        getBound(box, crit, cons, -1);
    }

    void getUpperBound(const Box<FT>& box, FT* crit, FT* cons) const {
        getBound(box, crit, cons, 1);
    }

    /**
     * Return references to the optioons
     * @return options
     */
    unsigned int& getOptions() {
        return mOptions;
    }

private:

    void getBound(const Box<FT>& box, FT* crit, FT* cons, int ul) const {
        FT r = BoxUtils::radius(box);
        BoxUtils::getCenter(box, (FT*)mBoxCenter);
        if (crit) {
            int critd = mMD->mCriteria.size();
            for (int i = 0; i < critd; i++) {
                FT cv = mMD->mCriteria[i]->func(mBoxCenter);
                crit[i] = bound(box, mMD->mCriteria[i], r, cv, ul);
            }
        }
        if (cons) {
            int consd = mMD->mConstraints.size();
            for (int i = 0; i < consd; i++) {
                FT cv = mMD->mConstraints[i]->func(mBoxCenter);
                cons[i] = bound(box, mMD->mConstraints[i], r, cv, ul);
            }
        }
    }

    FT bound(const Box<FT>& box, Objective<FT>* obj, FT r, FT cv, int ul) const {
        FT bnd = (ul == 1) ? ( std::numeric_limits< FT >::max()) : (-std::numeric_limits< FT >::max());

        FT b;
        if (mOptions & Options::LIP_FUNC) {
            b = lipFuncBound(box, obj, r, cv, ul);
            bnd = (ul == 1) ? BNBMIN(bnd, b) : BNBMAX(bnd, b);
        }
        if (mOptions & Options::LIP_GRAD) {
            b = lipGradBound(box, obj, r, cv, ul);
            bnd = (ul == 1) ? BNBMIN(bnd, b) : BNBMAX(bnd, b);
        }
        return bnd;
    }

    FT lipFuncBound(const Box<FT>& box, Objective<FT>* obj, FT r, FT cv, int ul) const {
        LipzObjective<double>* po = dynamic_cast<LipzObjective<double>*> (obj);
        BNB_ASSERT(po);
        FT l = po->getFuncLipzConstant(box.mA, box.mB);
        return cv + ul * l * r;
    }

    FT lipGradBound(const Box<FT>& box, Objective<FT>* obj, FT r, FT cv, int ul) const {
        LipzObjective<double>* po = dynamic_cast<LipzObjective<double>*> (obj);
        BNB_ASSERT(po);
        FT emin, emax;
        FT b = cv;
        int n = box.mDim;
        po->grad(mBoxCenter, mGrad);
        for (int i = 0; i < n; i++) {
            b += ul * BNBABS(mGrad[i] * (box.mB[i] - mBoxCenter[i]));
        }
        po->getEigenBounds(box.mA, box.mB, emin, emax);
        FT e = (ul == 1) ? emax : emin;

        b += 0.5 * e * r * r;
        //printf("emin = %lf, emax = %lf, e = %lf, b = %lf\n", emin, emax, e, b);
        return b;

    }


    MultiData<FT>* mMD;
    SmartArrayPtr<FT> mGrad;
    SmartArrayPtr<FT> mBoxCenter;
    unsigned int mOptions;
};

#endif	/* LIPBOUNDER_HPP */

