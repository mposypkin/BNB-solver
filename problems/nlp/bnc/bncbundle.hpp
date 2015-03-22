/* 
 * File:   bncbundle.hpp
 * Author: medved
 *
 * Created on February 12, 2015, 1:43 PM
 */

#ifndef BNCBUNDLE_HPP
#define	BNCBUNDLE_HPP

#include <problems/optlib/objective.hpp>
#include <problems/nlp/cuts/lpzsupp.hpp>
#include <problems/nlp/cuts/spectbndssupp.hpp>

/**
 * Bundle of all necessary things to produce cuts
 */
template <class FT> struct BNCBundle {
    
    /**
     * Objective function to optimize
     */
    Objective<FT>* mObjective;
    
    /**
     * Supplier of Lipschitz constant upper bound
     */    
    LpzSupplier<FT> *mLpzBoundSupp;
    
    /**
     * Spectrum bounds supplier
     */
    SpectrumBoundsSupplier<FT> *mSpectBoundsSupp;  
    
};

#endif	/* BNCBUNDLE_HPP */

