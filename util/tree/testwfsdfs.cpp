/* 
 * File:   testbnbtree.cpp
 * Author: medved
 *
 * Created on January 8, 2015, 6:24 PM
 */

#include <cstdlib>
#include <sstream>
#include "bnbtree.hpp"
#include "bnbtreeutils.hpp"
#include "wfsdfsmanager.hpp"

class IntPrinter : public BNBTreeUtils::BNBDataPrinter {
public:

    std::string printData(void* data) const {
        std::ostringstream os;
        os << *(int*) data;
        return os.str();
    }
};

class IntDeleter : public BNBTreeUtils::BNBDataDeleter {
public:

    void del(void* data) const {
        delete (int*) data;
    }
};

BNBNode* makeNode(int dat) {
    BNBNode* node = new BNBNode;
    node->mData = new int (dat);
    return node;
}

/*
 * 
 */
int main(int argc, char** argv) {
    BNBTree tree(makeNode(0));
    BNBNode* root = tree.getRoot();
    WFSDFSManager manager;
    manager.reg(root);
    int N = 8, D = 2;
    for (int i = 1; i < N;) {
        BNBNode* node = manager.get();
        BNBNode* nn;
        for (int j = 0; j < D; j++) {
            nn = makeNode(i++);
            BNBTreeUtils::appendNode(node, nn);
            manager.reg(nn);
        }
    }
    IntPrinter ip;
    BNBTreeUtils::printTree(tree, ip);


    manager.setOptions(WFSDFSManager::Options::DFS);
    for (int i = N; i < 2 * N;) {
        BNBNode* node = manager.get();
        BNBNode* nn;
        for (int j = 0; j < D; j++) {
            nn = makeNode(i++);
            BNBTreeUtils::appendNode(node, nn);
            manager.reg(nn);
        }
    }

    
    BNBTreeUtils::printTree(tree, ip);


    return 0;
}

