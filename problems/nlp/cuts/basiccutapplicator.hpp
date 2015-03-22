/* 
 * File:   basiccutapplicator.hpp
 * Author: medved
 *
 * Created on December 21, 2014, 1:42 PM
 */

#ifndef BASICCUTAPPLICATOR_HPP
#define	BASICCUTAPPLICATOR_HPP

#include <util/box/boxutils.hpp>
#include "cututils.hpp"
#include "cutapplicator.hpp"

/**
 * The straightforward cut applicator - removes the box if it is completely inside the cut or leaves it intact
 */
template <class FT> class BasicCutApplicator : public CutApplicator <FT> {
public:

    void ApplyCut(const std::vector< Cut<FT> > &cuts, const Box<FT> &box, std::vector< Box<FT> > &v) {
        bool rv = false;        
        for (auto cut : cuts) {
            if (CutUtils<FT>::isIn(cut, box)) {
                rv = true;
                break;
            }
        }
        if (!rv)
            v.push_back(box);
    }
};

#endif	/* BASICCUTAPPLICATOR_HPP */

