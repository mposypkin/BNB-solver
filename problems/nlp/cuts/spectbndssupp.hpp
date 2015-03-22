/* 
 * File:   SpectrumBoundSupplier.hpp
 * Author: medved
 *
 * Created on February 9, 2015, 8:42 PM
 */

#ifndef SPECTRUMBOUNDSUPPLIER_HPP
#define	SPECTRUMBOUNDSUPPLIER_HPP
#include <util/box/box.hpp>
/**
 * Retrieve spectrum bounds
 */
template <class FT> class SpectrumBoundsSupplier {
    public:
        
        /**
         * Retrieve Hessian spectrum bounds
         * @param box to compute bounds
         * @param lb lower bound (NULL means the bound is not needed)
         * @param ub upper bouns (NULL means the bound is not needed)
         */
        virtual void getBounds(const Box<FT>& box, FT* lb, FT* ub) = 0;
};

#endif	/* SPECTRUMBOUNDSUPPLIER_HPP */

