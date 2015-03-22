/* 
 * File:   recstore.hpp
 * Author: medved
 * Stores incumbents (records)
 * Created on January 3, 2015, 3:47 PM
 */

#ifndef RECSTORE_HPP
#define	RECSTORE_HPP

#include <util/common/smartptr.hpp>
#include <util/common/vec.hpp>

template <class FT> class RecStore {
public:

    /**
     * Retrieve the record value
     * @return the record value
     */
    virtual FT getValue() const = 0; 

    /**
     * Update the record
     * @param rec new record value
     * @param x new record vector
     * @return true if record was updated
     */
    virtual bool update(FT rec, FT* x) = 0;

};

#endif	/* RECSTORE_HPP */

