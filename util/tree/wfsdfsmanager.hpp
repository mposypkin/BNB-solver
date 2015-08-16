/* 
 * File:   wfsdfsmanager.hpp
 * Author: medved
 *
 * Created on January 9, 2015, 1:21 PM
 */

#ifndef WFSDFSMANAGER_HPP
#define	WFSDFSMANAGER_HPP

#include <set>
#include "bnbtreemanager.hpp"

/**
 * Node comparator
 */
class BNBNodeCmp {
public:

    bool operator()(const BNBNode* n1, const BNBNode* n2) const {
        return (n1->mLayer > n2->mLayer);
    }
};

/**
 * Manager that can switch between Width-first and Depth-first strategies
 */
class WFSDFSManager : public BNBTreeManager {
public:
    typedef std::multiset< BNBNode*, BNBNodeCmp > QType;

    struct Options {
        /**
         * Width first strategy
         */
        static const unsigned int WFS = 1;
        /**
         * Depth first strategy
         */
        static const unsigned int DFS = 2;
    };

    /**
     * Constructor 
     * @param deleter deleter class for nodes
     * @param opt initial options
     */
    WFSDFSManager(unsigned int opt = Options::WFS) {
        mOptions = opt;
    }

    /**
     * Setup options
     * @param opt options to set
     */
    void setOptions(unsigned int opt) {
        mOptions = opt;
    }

    /**
     * Inform tree manager about added node to the tree
     * @param node pointer to the added node
     */
    void reg(BNBNode* node) {
        mNodes.insert(node);
    }

    /**
     * Retrieves next node from the tree 
     * @return pointer to the node data
     */
    BNBNode* get() {
        if (mNodes.empty())
            return NULL;
        typename QType::iterator i;
        if (mOptions == Options::WFS) {
            i = mNodes.end();
            i--;
        } else {
            i = mNodes.begin();
        }
        BNBNode* n = *i;
        mNodes.erase(i);
        return n;
    }

    /**
     * Traverses the node list
     * @param handler handles the nodes
     */
    void traverse(std::function<void (BNBNode*) > handler) {
        if (mOptions == Options::WFS) {
            for (auto i = mNodes.rbegin(); i != mNodes.rend(); i++) {
                handler(*i);
            }
        } else {
            for (auto i = mNodes.begin(); i != mNodes.end(); i++) {
                handler(*i);
            }
        }
    }

    /**
     * Retrieve the number of registered nodes
     * @return number of nodes
     */
    long unsigned int size() const {
        return mNodes.size();
    }

private:
    QType mNodes;
    unsigned int mOptions;
};


#endif	/* WFSDFSMANAGER_HPP */

