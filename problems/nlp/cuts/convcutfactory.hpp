/* 
 * File:   convcutfactory.hpp
 * Author: medved
 *
 * Created on February 9, 2015, 10:08 PM
 */

#ifndef CONVCUTFACTORY_HPP
#define	CONVCUTFACTORY_HPP

#include "cutfactory.hpp"
#include "spectbndssupp.hpp"
#include "locsearch.hpp"

/**
 * Convexity based cut factory (for box-constrained optimization
 */
template <class FT> class ConvCutFactory : public CutFactory <FT> {
public:

    /**
     * The counstructor 
     * @param rs the record storage
     * @param supp the supplier of Lipschitz constant
     * @param objc objective
     */
    ConvCutFactory(RecStore<FT>* rs, SpectrumBoundsSupplier<FT>* supp, Objective<FT>* obj, LocSearch<FT>* ls)
    : mRecStore(rs), mSupp(supp), mObj(obj), mLS(ls) {
    }

    void getCuts(const Box<FT>& box, std::vector< Cut<FT> >& cuts) {
        int n = box.mDim;
        SmartArrayPtr<FT> z(n);
        BoxUtils::getCenter(box, (FT*) z);
        FT fv = mObj->func(z);
        mRecStore->update(fv, (FT*) z);
        FT fr = mRecStore->getValue();
        FT k, K;
        mSupp->getBounds(box, &k, &K);

        Cut<FT> cut;
        if ((k >= 0) || (K <= 0)) {
            fv = mLS->search(z);
            mRecStore->update(fv, (FT*) z);
            cut.mType = Cut<FT>::CutType::TOTAL;
            cuts.push_back(cut);
        }
    }


private:
    SpectrumBoundsSupplier<FT>* mSupp;
    RecStore<FT>* mRecStore;
    Objective<FT>* mObj;
    LocSearch<FT>* mLS;
};



#endif	/* CONVCUTFACTORY_HPP */

