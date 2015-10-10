/* 
 * File:   bncsolver.hpp
 * Author: medved
 *
 * Basic Branch and Cut Solver
 * 
 * Created on January 6, 2015, 2:20 PM
 */

#ifndef BNCSOLVER_HPP
#define	BNCSOLVER_HPP

#include <util/box/boxutils.hpp>
#include <util/tree/bnbtreeutils.hpp>
#include <problems/nlp/cuts/cutapplicator.hpp>
#include <problems/nlp/cuts/cutfactory.hpp>
#include "bncstate.hpp"
#include "bncsubprinter.hpp"
#include "boxsplitter.hpp"

template <class FT> class BNCSolver {
public:

    /**
     * Constructor
     * @param ca pointer to cut applicator
     * @param cf pointer to cut factory
     * @param splt pointer to box splitter
     * @param clpd cut lookup depth (how long go up to the tree for cuts)
     */
    BNCSolver(CutFactory<FT>* cf, CutApplicator<FT>* ca, BoxSplitter<FT>* splt, int clpd = 1) {
        mCutFactory = cf;
        mCutApplicator = ca;
        mCutLookupDepth = clpd;
        mBoxSplitter = splt;
    }

    /**
     * The resolution process
     * @param maxiters the upper bound for iterations on entry, on exit real number of performed iters
     * @param bncs the state of computations 
     * @param recupd true if record was updated
     */
    void solve(long long int& maxiters, BNCState<FT> & bnc, bool& recupd) {
        long long int I = 0;
        BNCSubPrinter<double> subprinter;
        for (; I < maxiters; I++) {
#if 0            
            std::cout << "step " << I << " ^^^^^^^^^^^^^^^^^^\n";
            std::cout << "Record = " << bnc.mRecord->getValue() << "\n";
#endif
#if 0 
            BNBTreeUtils::printTree(*bnc.mTree, subprinter);
            std::cout << "step " << I << " vvvvvvvvvvvvvvvvvv\n";
#endif            


            BNBNode* node = bnc.mTreeManager->get();
            if (node == NULL)
                break;
            BNCSub<FT>* sub = (BNCSub<FT>*) node->mData;


            mCutFactory->getCuts(sub->mBox, sub->mCuts);


            std::vector< Box<FT> > bv;
            std::vector< Cut<FT> > cuts;
            BNBNode* np = node;
            int cutd = 0;
            int n = sub->mBox.mDim;
            bv.push_back(sub->mBox);

            while (np && (cutd++ < mCutLookupDepth)) {
#if 0
                std::cout << "Cuts at node " << cutd << ":\n";
#endif                
                BNCSub<FT>* subp = (BNCSub<FT>*) np->mData;
                applyCuts(subp->mCuts, bv);
                if (bv.empty())
                    break;
                np = np->mParent;
            }
            if (bv.empty()) {
                deleteNode(node);
                continue;
            } else if (bv.size() == 1) {
                Box<FT> b0(n), b1(n), b2(n);
                b0 = bv.at(0);
                bv.pop_back();
                /*
                BoxUtils::divideByLongestEdge(b0, b1, b2);
                bv.push_back(b1);
                bv.push_back(b2);
                 */
                mBoxSplitter->split(b0, bv);
            }
            pushNewSubs(*bnc.mTreeManager, bv, node);
        }
        maxiters = I;
    }

private:

    void deleteNode(BNBNode* node) {
        BNB_ASSERT(node->mData != NULL);
        BNBNode* next = node->mNext;
        BNBNode* prev = node->mPrev;
        BNBNode* par = node->mParent;
        /* Deletes data */
        delete node->mData;
        delete node;
        /* Check if the last child */
        if ((next == NULL) && (prev == NULL)) {
            if (par)
                deleteNode(par);
        } else {
            if (next != NULL) {
                next->mPrev = prev;
            }
            if (prev != NULL) {
                prev->mNext = next;
            }
        }
    }

    void applyCuts(const std::vector< Cut<FT> > &cuts, std::vector< Box<FT> > &v) {
        std::vector< Box<FT> > nv;
        for (auto b : v) {
#if 0          
            for (auto c : cuts) {
                std::cout << "Applying cuts: ";
                std::cout << CutUtils<FT>::toString(c) << " to box " << BoxUtils::toString(b) << "\n";
            }
#endif
            mCutApplicator->ApplyCut(cuts, b, nv);
#if 0       
            if (nv.size() == 0) {
                std::cout << "Total elimination\n";
            } else if ((nv.size() == 1) && BoxUtils::isSameBox(b, nv.at(0))) {
                std::cout << "no effect\n";
            } else {
                std::cout << "Resulting boxes: \n";
                for (auto bb : nv) {
                    std::cout << "Box: " << BoxUtils::toString(bb) << "\n";
                }
            }

#endif            
        }
        v = nv;
    }

    void pushNewSubs(BNBTreeManager& manager, const std::vector< Box<FT> > &bv, BNBNode * node) {
        for (auto o : bv) {
            BNBNode* nn = makeNode(o);
            BNBTreeUtils::appendNode(node, nn);
            manager.reg(nn);
        }
    }

    BNBNode * makeNode(const Box<FT>& box) {
        BNBNode* node = new BNBNode;
        BNCSub<double>* sub = new BNCSub<double>();
        sub->mBox = box;
        node->mData = sub;
        return node;
    }
    CutFactory<FT>* mCutFactory;
    CutApplicator<FT>* mCutApplicator;
    BoxSplitter<FT>* mBoxSplitter;
    int mCutLookupDepth;


};


#endif	/* BNCSOLVER_HPP */

