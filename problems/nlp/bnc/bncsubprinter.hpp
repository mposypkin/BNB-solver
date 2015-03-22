/* 
 * File:   bncsubprinter.hpp
 * Author: medved
 *
 * Created on January 11, 2015, 1:47 PM
 */

#ifndef BNCSUBPRINTER_HPP
#define	BNCSUBPRINTER_HPP

#include <sstream>
#include <util/box/boxutils.hpp>
#include <util/tree/bnbtreeutils.hpp>
#include <problems/nlp/cuts/cututils.hpp>
#include "bncsub.hpp"

template <class FT> class BNCSubPrinter : public BNBTreeUtils::BNBDataPrinter {
public:

    std::string printData(void* data) const {        
        std::ostringstream os;
        BNCSub<FT>* sub = (BNCSub<FT>*) data;
        os << "Box: " << BoxUtils::toString(sub->mBox);
        os << "\n";
        os << sub->mCuts.size() <<" cuts:\n";
        for(auto o : sub->mCuts) {
            os << CutUtils<FT>::toString(o);
        }
        return os.str();
    }
};


#endif	/* BNCSUBPRINTER_HPP */

