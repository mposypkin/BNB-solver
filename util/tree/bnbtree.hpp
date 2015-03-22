/* 
 * File:   bnbtree.hpp
 * Author: medved
 *
 * Created on January 7, 2015, 8:57 PM
 */

#ifndef BNBTREE_HPP
#define	BNBTREE_HPP

#include "bnbnode.hpp"

/**
 * Tree to store nodes of BnB tree
 */
struct BNBTree {
public:

    /**
     * Constructor
     * @param node root node
     */
    BNBTree(BNBNode* node) {
        mRoot = node;
    }

    /**
     * Retrieve the tree root
     * @return tree root
     */
    BNBNode* getRoot() const {
        return mRoot;
    }


private:

    BNBNode* mRoot;
};

#endif	/* BNBTREE_HPP */

