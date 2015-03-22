/* 
 * File:   smartcutapplicator.hpp
 * Author: medved
 *
 * Created on January 16, 2015, 9:57 PM
 */

#ifndef SMARTCUTAPPLICATOR_HPP
#define	SMARTCUTAPPLICATOR_HPP

#include <vector>

#include "util/box/boxutils.hpp"
#include "cutapplicator.hpp"
#include "cututils.hpp"

/**
 * The straightforward cut applicator - removes the box if it is completely inside the cut or leaves it intact
 */
template <class FT> class SmartCutApplicator : public CutApplicator <FT> {
public:

    void ApplyCut(const std::vector< Cut<FT> > &cuts, const Box<FT> &box, std::vector< Box<FT> > &v) {
        std::vector< Box<FT> > u;
        u.push_back(box);
        for (auto cut : cuts) {
            std::vector< Box<FT> > nv;
            for (auto b : u) {
                CutUtils<FT>::ApplyCut(cut, b, nv);
            }
            u = nv;
        }
        v.insert(v.end(), u.begin(), u.end());
    }

};


#endif	/* SMARTCUTAPPLICATOR_HPP */

