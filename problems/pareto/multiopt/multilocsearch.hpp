/* 
 * File:   multilocsearch.hpp
 * Author: posypkin
 *
 * Created on April 23, 2011, 12:43 PM
 */

#ifndef MULTILOCSEARCH_HPP
#define	MULTILOCSEARCH_HPP

#include "multidata.hpp"
#include "multipoint.hpp"

template <typename FT> class MultiLocalSearch {
public:

    /**
     * Constructor
     * @param md problem data
     */
    MultiLocalSearch(MultiData<FT> md) : mMD(md) {        
    }

    /**
     * Performs a local search from the given point
     * @param mp starting point
     */
    virtual void search(MultiPoint<FT>& mp) = 0;

protected:
    MultiData<FT> mMD;
};


#endif	/* MULTILOCSEARCH_HPP */

