/* 
 * File:   compcutfactory.hpp
 * Author: medved
 *
 * Created on February 6, 2015, 8:47 PM
 */

#ifndef COMPCUTFACTORY_HPP
#define	COMPCUTFACTORY_HPP

#include "cutfactory.hpp"

/**
 * Composite cut factory for applying several cut techniques
 */
template <class FT> class CompCutFactory : public CutFactory <FT> {
public:

    CompCutFactory() {
        
    }
    
    /**
     * Appends cut factory
     * @param cutf new cut factory
     */
    void push(CutFactory<FT>* cutf) {
        mCutFV.push_back(cutf);
    }
    
    void getCuts(const Box<FT>& box, std::vector< Cut<FT> >& cuts) {
        for(auto o : mCutFV) {
            o->getCuts(box, cuts);
        }
    }
    
private:
    std::vector< CutFactory<FT>* > mCutFV;
};

#endif	/* COMPCUTFACTORY_HPP */

