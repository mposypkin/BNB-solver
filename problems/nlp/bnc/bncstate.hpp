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
     * @param tree pointer to branch-and-cut tree
     * @param manager pointer to the tree manager
     * @param rec pointer to the record storage
     */
    BNCState(BNBTree* tree, BNBTreeManager* manager, RecStore<FT>* rec) {
        mTree = tree;
        mTreeManager = manager;
        mRecord = rec;
    }
    
    /**
     * Branch-and-cut tree
     */
    BNBTree* mTree;
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

