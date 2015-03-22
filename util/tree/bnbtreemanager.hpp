/* 
 * File:   bnbtreemanager.hpp
 * Author: medved
 *
 * Created on January 9, 2015, 12:36 PM
 */

#ifndef BNBTREEMANAGER_HPP
#define	BNBTREEMANAGER_HPP

#include "bnbtree.hpp"

/**
 * Interface for managing  of BNBTrees
 */
class BNBTreeManager {
public:

    /**
     * Inform tree manager about added node to the tree
     * @param node pointer to the added node
     */
    virtual void reg(BNBNode* node) = 0;
    
    /**
     * Retrieves next node from the tree 
     * @return pointer to the node data
     */
    virtual BNBNode* get() = 0;
    
 };

#endif	/* BNBTREEMANAGER_HPP */

