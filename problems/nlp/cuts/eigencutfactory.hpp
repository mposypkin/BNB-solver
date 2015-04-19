/* 
 * File:   eigencutfactory.hpp
 * Author: medved
 *
 * The cut factory based on eigenvalue estimates
 * Created on January 20, 2015, 3:17 PM
 */

#ifndef EIGENCUTFACTORY_HPP
#define	EIGENCUTFACTORY_HPP

#include <vector>
#include <math.h>

#include <problems/optlib/objective.hpp>
#include <util/box/boxutils.hpp>
#include "cutfactory.hpp"
#include "recstore.hpp"
#include "cututils.hpp"
#include "spectbndssupp.hpp"

/**
 * Cuts based on eigenvector  underestimators
 */
template <class FT> class EigenCutFactory : public CutFactory <FT> {
public:


    /**
     * The counstructor 
     * @param rs the record storage
     * @param supp the supplier of Lipschitz constant
     * @param objc objective
     * @param eps precision
     */
    EigenCutFactory(RecStore<FT>* rs, SpectrumBoundsSupplier<FT>* supp, Objective<FT>* obj, FT eps = 0)
    : mRecStore(rs), mSupp(supp), mObj(obj), mEps(eps) {
    }

    void getCuts(const Box<FT>& box, std::vector< Cut<FT> >& cuts) {
        int n = box.mDim;
        SmartArrayPtr<FT> z(n);
        BoxUtils::getCenter(box, (FT*) z);
        FT fv = mObj->func(z);
        mRecStore->update(fv, (FT*) z);
        SmartArrayPtr<FT> g(n);
        mObj->grad(z, g);
        FT fr = mRecStore->getValue();
        FT k;
        mSupp->getBounds(box, &k, NULL);
        
        std::cout << "eigen k = " << k << " for a box " << BoxUtils::toString(box) << "\n";


        Cut<FT> cut;
        if (k == 0) {
            cut.mR = fv - fr + mEps;
            cut.mC = g;
            cut.mType = Cut<FT>::CutType::LINEAR;
        } else {
            FT a = fr - fv - mEps + (1. / (2 * k)) * VecUtils::vecNormTwoSqr(n, (FT*) g);
            SmartArrayPtr<FT> c(n);
            VecUtils::vecSaxpy(n, (FT*) z, (FT*) g, -1 / k, (FT*) c);
            cut.mC = c;
            if (k > 0) {
                if (a <= 0) {
                    cut.mType = Cut<FT>::CutType::TOTAL;
                } else {
                    cut.mR = sqrt(2 * a / k);
                    cut.mType = Cut<FT>::CutType::OUTER_BALL;
                }
            } else {
                cut.mR = sqrt(2 * a / k);
                cut.mType = Cut<FT>::CutType::INNER_BALL;
            }
        }
        cuts.push_back(cut);
    }


private:
    SpectrumBoundsSupplier<FT>* mSupp;
    RecStore<FT>* mRecStore;
    Objective<FT>* mObj;
    FT mEps;
};

#endif	/* EIGENCUTFACTORY_HPP */

