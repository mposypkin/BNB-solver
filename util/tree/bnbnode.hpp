/* 
 * File:   bnbnode.hpp
 * Author: medved
 *
 * Created on January 7, 2015, 8:58 PM
 */

#ifndef BNBNODE_HPP
#define	BNBNODE_HPP

struct BNBNode {
    /**
     * Constructor
     */
    BNBNode(void* data = NULL) {
        mData = data;
        mParent = NULL;
        mNext = NULL;
        mPrev = NULL;
        mChildren = NULL;
        mLayer = 0;
    }
    /**
     * Pointer to the parent (NULL for the top element element)
     */
    BNBNode* mParent;
    /**
     * Pointer to the next sibling (NULL for the last sibling)
     */
    BNBNode* mNext;
    /**
     * Pointer to the previous sibling (NULL for the first sibling)
     */
    BNBNode* mPrev;
    /**
     * Pointer to the list of children (NULL for the first sibling)
     */
    BNBNode* mChildren;
    /**
     * The layer number in a tree (the length of path from the root, root has 0 layer)
     */
    long long int mLayer;
    /**
     * The data
     */
    void* mData;
};

#endif	/* BNBNODE_HPP */

