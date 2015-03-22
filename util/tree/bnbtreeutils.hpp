/* 
 * File:   bnbtreeutils.hpp
 * Author: medved
 *
 * Created on January 8, 2015, 6:03 PM
 */

#ifndef BNBTREEUTILS_HPP
#define	BNBTREEUTILS_HPP

#include <iostream>
#include <string>

#include "bnbtree.hpp"

/**
 * Utils to work with Branch-and-Bound trees
 */
class BNBTreeUtils {
public:

    /**
     * Printer for the data associated with the node
     * @param data pointer to the data
     */
    class BNBDataPrinter {
    public:
        /**
         * Prints data
         * @param data pointer to the data to print
         * @return the string representation of the data to print
         */
        virtual std::string printData(void* data) const = 0;
    };

    /**
     * Deteter for the data associated with the node
     */
    class BNBDataDeleter {
    public:
        /**
         * Deletes the data associated with the pointer
         */
        virtual void del(void* data) const = 0;
    };

    /**
     * Print tree 
     * @param tree tree to print
     * @param printer data printer
     */
    static void printTree(const BNBTree& tree, const BNBDataPrinter& printer) {
        BNBNode* root = tree.getRoot();
        printNode(root, printer, "");
    }

    /**
     * Print node 
     * @param node tree to print
     * @param printer data printer
     * @param shift offset
     */
    static void printNode(const BNBNode* node, const BNBDataPrinter& printer, std::string shift = "") {
        if (node == NULL)
            return;
        if (node->mParent == NULL) {
            std::cout << shift << "Root[" << node->mLayer << "]\n";
        } else {
            std::cout << shift << "Node[" << node->mLayer << "]\n";
        }
        std::cout << shift << "Data: " << printer.printData(node->mData) << "\n";
        inc(shift);
        printNode(node->mChildren, printer, shift);
        dec(shift);
        printNode(node->mNext, printer, shift);

    }

    /**
     * Delete the branch
     * @param node root node of a branch
     */
    static void deleteSubTree(BNBNode* node, const BNBDataDeleter& ddel) {
        if (node == NULL)
            return;
        deleteLayer(node->mChildren, ddel);
        ddel.del(node->mData);
        if (node->mPrev != NULL) {
            node->mPrev->mNext = node->mNext;
        } else if (node->mParent != NULL) {
            node->mParent->mChildren = node->mNext;
        }
        if (node->mNext != NULL)
            node->mNext->mPrev = node->mPrev;
        delete node;
    }

    /**
     * Appends node to the pending branch of a tree
     * @param branch branch to append the node
     * @param node node to append
     */
    static void appendNode(BNBNode* branch, BNBNode* node) {
        BNBNode* children = branch->mChildren;
        branch->mChildren = node;
        node->mParent = branch;
        node->mLayer = branch->mLayer + 1;
        if (children != NULL) {
            children->mPrev = node;
            node->mNext = children;
        }
    }
private:

    static void deleteLayer(BNBNode* node, const BNBDataDeleter& ddel) {
        if (node == NULL)
            return;
        deleteSubTree(node->mChildren, ddel);
        deleteSubTree(node->mNext, ddel);
        ddel.del(node->mData);
        delete node;
    }

    static void inc(std::string& shift) {
        for (int i = 0; i < mShiftNum; i++)
            shift.push_back(' ');
    }

    static void dec(std::string& shift) {
        for (int i = 0; i < mShiftNum; i++)
            shift.pop_back();
    }

    static const int mShiftNum = 2;
};

#endif	/* BNBTREEUTILS_HPP */

