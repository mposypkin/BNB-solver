/* 
 * File:   pointcutfactory.hpp
 * Author: medved
 *
 * Created on June 10, 2015, 3:09 PM
 */

#ifndef POINTCUTFACTORY_HPP
#define	POINTCUTFACTORY_HPP

#include <iostream>

#include "cutfactory.hpp"
#include "recstore.hpp"


/**
 * Eliminate degenerate boxes which are in fact points
 */
template <class FT> class PointCutFactory : public CutFactory <FT> {
public:

    /**
     * The constructor 
     * @param rs the record storage
     */
    PointCutFactory(RecStore<FT>* rs)
    : mRecStore(rs) {
    }

    void getCuts(const Box<FT>& box, std::vector< Cut<FT> >& cuts) {
        int n = box.mDim;
        for (int i = 0; i < n; i++) {
            if (box.mA[i] != box.mB[i]) {
                return;
            }
        }       
        mRecStore->update(0, (FT*) box.mA);
        Cut<FT> cut;
        cut.mType = Cut<FT>::CutType::TOTAL;
        cuts.push_back(cut);
    }

private:
    RecStore<FT>* mRecStore;
    
};



#endif	/* POINTCUTFACTORY_HPP */

