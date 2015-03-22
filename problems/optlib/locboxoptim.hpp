/* 
 * File:   locboxoptim.hpp
 * Author: posypkin
 *
 * Base class for optimizers with box constraints
 *
 * Created on April 23, 2011, 8:27 AM
 */

#ifndef LOCBOXOPTIM_HPP
#define	LOCBOXOPTIM_HPP

#include "locoptim.hpp"
#include "util/box/box.hpp"

template <typename FT> class LocalBoxOptimizer : public LocalOptimizer <FT> {
public:

    /**
     * Constructor
     * @param box bounding box
     */
    LocalBoxOptimizer(Box<FT>& box) : LocalOptimizer<FT> (), mBox(box){

    }

    /**
     * Change the bounding box
     * @param box new bounding box
     */
    void setBox(Box<FT>& box) {
        mBox = box;
    }

protected:

    Box<FT> mBox;

};


#endif	/* LOCBOXOPTIM_HPP */

