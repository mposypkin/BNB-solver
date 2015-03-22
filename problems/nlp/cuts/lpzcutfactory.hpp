/* 
 * File:   LpzCutFactory.hpp
 * Author: medved
 *
 * Created on November 1, 2014, 9:15 PM
 */

#ifndef LPZCUTFACTORY_HPP
#define	LPZCUTFACTORY_HPP

#include <problems/optlib/objective.hpp>
#include <util/box/boxutils.hpp>
#include "cutfactory.hpp"
#include "recstore.hpp"
#include "lpzsupp.hpp"

template <class FT> class LpzCutFactory : public CutFactory <FT> {
public:


    /**
     * The counstructor 
     * @param rs the record storage
     * @param supp the supplier of Lipschitz constant
     * @param objc objective
     * @param eps precision
     */
    LpzCutFactory(RecStore<FT>* rs, LpzSupplier<FT>* supp, Objective<FT>* obj, FT eps = 0)
    : mRecStore(rs), mSupp(supp), mObj(obj), mEps(eps) {
    }

    void getCuts(const Box<FT>& box, std::vector< Cut<FT> >& cuts) {
        int n = box.mDim;
        SmartArrayPtr<FT> ptr(n);
        BoxUtils::getCenter(box, (FT*) ptr);
        FT fv = mObj->func(ptr);
        mRecStore->update(fv, (FT*)ptr);
        FT fr = mRecStore->getValue();
        FT l = mSupp->getLpzConst(box);
        FT r = (fv - fr + mEps) / l;
        Cut<FT> cut;
        cut.mR = r;
        cut.mC = ptr;
        cut.mType = Cut<FT>::CutType::INNER_BALL;
        cuts.push_back(cut);
    }


private:
    LpzSupplier<FT>* mSupp;
    RecStore<FT>* mRecStore;
    Objective<FT>* mObj;
    FT mEps;
};

#endif	/* LPZCUTFACTORY_HPP */

