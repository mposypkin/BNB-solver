/* 
 * File:   lpzsupp.hpp
 * Author: medved
 *
 * Created on February 12, 2015, 1:32 PM
 */

#ifndef LPZSUPP_HPP
#define	LPZSUPP_HPP

#include <util/box/box.hpp>

/**
 * Supplies Lipschitz constant
 */
template <class FT> class LpzSupplier {
public:
    /**
     * Retrieve Lipschitz constant for a box
     * @param box working box
     * @return the value of Lipschitz constant
     */
    virtual FT getLpzConst(const Box<FT>& box) = 0;
};



#endif	/* LPZSUPP_HPP */

