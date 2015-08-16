/* 
 * File:   bnbtreemanager.hpp
 * Author: medved
 *
 * Created on January 9, 2015, 12:36 PM
 */

#ifndef BNBTREEMANAGER_HPP
#define	BNBTREEMANAGER_HPP

#include <functional>

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
     * Retrieves and removes next node from the tree 
     * @return pointer to the node data
     */
    virtual BNBNode* get() = 0;
    
    /**
     * Retrieve the number of registered nodes
     * @return number of nodes
     */
    virtual long unsigned int size() const = 0;
   
    /**
     * Traverses the node list
     * @param handler handles the nodes
     */
    virtual void traverse(std::function<void (BNBNode*)> handler) = 0;
    
 };

#endif	/* BNBTREEMANAGER_HPP */

