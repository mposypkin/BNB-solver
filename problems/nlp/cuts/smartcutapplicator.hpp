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
                applyCut(cut, b, nv);
            }
            u = nv;
            if (u.empty()) {
                break;
            }
        }
        v.insert(v.end(), u.begin(), u.end());
    }

private:

    void applyCut(const Cut<FT>& cut, const Box<FT>& box, std::vector< Box<FT> >& v) {
        if (cut.mType == Cut<FT>::CutType::INNER_BALL) {
#if 0
            CutUtils<FT>::ApplyInnerBallCutSimple(cut, box, v);
#endif            
#if 0            
            CutUtils<FT>::ApplyInnerBallCutBoxed(cut, box, v);
#endif            
#if 1
            if (!CutUtils<FT>::ApplyInnerBallCutSimple(cut, box, v)) {
                v.clear();
                CutUtils<FT>::ApplyInnerBallCutBoxed(cut, box, v);
            }
#endif            
#if 0
            CutUtils<FT>::ApplyInnerBallCutSimple(cut, box, v);
            std::vector < Box < FT >> u;
            for (auto b : v) {
                CutUtils<FT>::ApplyInnerBallCutBoxed(cut, b, u);
            }
            v = u;
#endif            
        } else if (cut.mType == Cut<FT>::CutType::OUTER_BALL) {
            CutUtils<FT>::ApplyOuterBallCut(cut, box, v);
        } else if (cut.mType == Cut<FT>::CutType::LINEAR) {
            CutUtils<FT>::ApplyLinearCut(cut, box, v);
        } else if (cut.mType == Cut<FT>::CutType::TOTAL) {
            /** DO NOTHING */
        }
    }
};


#endif	/* SMARTCUTAPPLICATOR_HPP */

