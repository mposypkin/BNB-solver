/* 
 * File:   unconscutfactory.hpp
 * Author: medved
 *
 * Created on February 7, 2015, 4:00 PM
 */

#ifndef UNCONSCUTFACTORY_HPP
#define	UNCONSCUTFACTORY_HPP

#include "cutfactory.hpp"
#include "spectbndssupp.hpp"

/**
 * Cut factory for unconstrained optimization.
 */
template <class FT> class UnconsCutFactory : public CutFactory <FT> {
public:

    /**
     * The counstructor 
     * @param rs the record storage
     * @param supp the supplier of Lipschitz constant
     * @param objc objective
     * @param box initial feasible box
     * @param eps precision
     */
    UnconsCutFactory(RecStore<FT>* rs, SpectrumBoundsSupplier<FT>* supp, Objective<FT>* obj, Box<FT>* box, FT eps = 0)
    : mRecStore(rs), mSupp(supp), mObj(obj), mBox(box), mEps(eps) {
    }

    void getCuts(const Box<FT>& box, std::vector< Cut<FT> >& cuts) {
        int n = box.mDim;
        SmartArrayPtr<FT> z(n);
        BoxUtils::getCenter(box, (FT*) z);
        FT fv = mObj->func(z);
        mRecStore->update(fv, (FT*) z);
        FT fr = mRecStore->getValue();

        if (BoxUtils::isStrictSubBox(*mBox, box)) {
            FT k;
            mSupp->getBounds(box, NULL, &k);


            Cut<FT> cut;
            FT a = fv - fr + mEps;
            if (k <= 0) {
                cut.mType = Cut<FT>::CutType::TOTAL;
            } else {
                cut.mC = z;
                cut.mR = sqrt(2 * a / k);
                cut.mType = Cut<FT>::CutType::INNER_BALL;
            }
            cuts.push_back(cut);
        }
    }


private:
    SpectrumBoundsSupplier<FT>* mSupp;
    RecStore<FT>* mRecStore;
    Objective<FT>* mObj;
    Box<FT>* mBox;
    FT mEps;
};

#endif	/* UNCONSCUTFACTORY_HPP */

