/* 
 * File:   bncstate.hpp
 * Author: medved
 * 
 * The state of Branch-and-Cut algorithm
 *
 * Created on January 6, 2015, 1:09 PM
 */

#ifndef BNCSTATE_HPP
#define	BNCSTATE_HPP

#include <vector>
#include <util/tree/bnbtree.hpp>
#include <util/tree/bnbtreemanager.hpp>
#include <problems/nlp/cuts/recstore.hpp>

/**
 * The state of Branch-and-Cut Algorith
 */
template <class FT> struct BNCState {
    
    /**
     * Constructor
     * @param manager pointer to the tree manager
     * @param rec pointer to the record storage
     */
    BNCState(BNBTreeManager* manager, RecStore<FT>* rec) {
        mTreeManager = manager;
        mRecord = rec;
    }
    
    /**
     * Branch-and-cut forest
     */
    std::vector<BNBTree*> mForest;
    /**
     * Branch-and-cut tree manager
     */
    BNBTreeManager* mTreeManager;
    /**
     * Record storage
     */
    RecStore<FT>* mRecord;
};



#endif	/* BNCSTATE_HPP */

