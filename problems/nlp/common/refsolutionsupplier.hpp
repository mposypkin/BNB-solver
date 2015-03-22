/* 
 * File:   RefSolutionSupplier.hpp
 * Author: medved
 *
 * Created on February 12, 2015, 1:53 PM
 */

#ifndef REFSOLUTIONSUPPLIER_HPP
#define	REFSOLUTIONSUPPLIER_HPP

template <class FT> class RefSolutionSupplier {
    public:
        /**
         * Retrieve reference value and (possibly) solution
         * @param x place to store the reference solution (if NULL - store nothing)
         * @return reference objective value
         */
        virtual FT getSolution(FT* x) = 0;
};

#endif	/* REFSOLUTIONSUPPLIER_HPP */

