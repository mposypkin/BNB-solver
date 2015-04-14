/* 
 * File:   dummyrecstore.hpp
 * Author: medved
 *
 * Returns the specified value (zero by default) and never changes it 
 * Created on April 14, 2015, 3:12 PM
 */

#ifndef DUMMYRECSTORE_HPP
#define	DUMMYRECSTORE_HPP

template <class FT> class DummyRecStore : public RecStore <FT> {
public:

    /**
     * The constructor
     * @param rec the initial record value     
     */
    DummyRecStore(FT rec = 0) {
        mRec = rec;
    }

    /**
     * Retrieve the record value
     * @return the record value
     */
    FT getValue() const {
        return mRec;
    }
    /**
     * Update the record
     * @param rec new record value
     * @param x new record vector
     * @return true if record was updated
     */
    bool update(FT rec, FT* x) {
        return false;
    }


private:

    FT mRec;
};


#endif	/* DUMMYRECSTORE_HPP */

