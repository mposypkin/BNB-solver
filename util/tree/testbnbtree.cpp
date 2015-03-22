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

    BNBNode* node1, *node2;
    BNBTreeUtils::appendNode(root, node1 = makeNode(1));
    BNBTreeUtils::appendNode(root, node2 = makeNode(2));
    BNBTreeUtils::appendNode(node1, makeNode(3));
    BNBTreeUtils::appendNode(node1, makeNode(4));
    BNBTreeUtils::appendNode(node2, makeNode(5));
    BNBTreeUtils::appendNode(node2, makeNode(6));
    BNBTreeUtils::appendNode(node2, makeNode(7));

    IntPrinter ip;
    BNBTreeUtils::printTree(tree, ip);

    IntDeleter idel;  
    BNBTreeUtils::deleteSubTree(node2, idel);
    BNBTreeUtils::printTree(tree, ip);
    BNBTreeUtils::deleteSubTree(node1, idel);
    BNBTreeUtils::printTree(tree, ip);
    
    
    
    return 0;
}

