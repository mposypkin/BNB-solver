/* 
 * File:   smartcutapplicator.hpp
 * Author: medved
 *
 * Created on January 16, 2015, 9:57 PM 
 */

#ifndef SMARTCUTAPPLICATOR_HPP
#define	SMARTCUTAPPLICATOR_HPP

#include <vector>

#include <util/box/boxutils.hpp>
#include <problems/nlp/common/nlpproblem.hpp>
#include "cutapplicator.hpp"
#include "cututils.hpp"

/**
 * The straightforward cut applicator - removes the box if it is completely inside the cut or leaves it intact
 */
template <class FT> class SmartCutApplicator : public CutApplicator <FT> {
public:

    /** 
     * Options 
     */
    struct Options {
        /**
         * Cut a ball from a box
         */
        static const unsigned int CUT_BALL_SIMPLE = 1;

        /**
         * Cut the maximal box inscribed in a ball from the box
         */
        static const unsigned int CUT_BALL_BOXED = 1 << 1;
    };

    /**
     * Returns reference to options
     * @return ref to options 
     */
    unsigned int& getOptions() {
        return mOptions;
    }

    /**
     * Constructor 
     * @param vtypes a reference to the variable types (empty vector means all variables are generic)
     */
    SmartCutApplicator(const std::vector<unsigned int>& vtypes = std::vector<unsigned int>()) : mVTypes(vtypes) {
        mOptions = Options::CUT_BALL_SIMPLE;
    }

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
            if (mOptions == Options::CUT_BALL_SIMPLE) {
                CutUtils<FT>::ApplyInnerBallCutSimple(cut, box, mVTypes, v);
            } else if (mOptions == Options::CUT_BALL_BOXED) {
                CutUtils<FT>::ApplyInnerBallCutBoxed(cut, box, mVTypes, v);
            } else if (mOptions == (Options::CUT_BALL_SIMPLE | Options::CUT_BALL_BOXED))
                if (!CutUtils<FT>::ApplyInnerBallCutSimple(cut, box, mVTypes, v)) {
                    v.clear();
                    CutUtils<FT>::ApplyInnerBallCutBoxed(cut, box, mVTypes, v);
                }
        } else if (cut.mType == Cut<FT>::CutType::OUTER_BALL) {
            CutUtils<FT>::ApplyOuterBallCut(cut, box, mVTypes, v);
        } else if (cut.mType == Cut<FT>::CutType::LINEAR) {
            CutUtils<FT>::ApplyLinearCut(cut, box, mVTypes, v);
        } else if (cut.mType == Cut<FT>::CutType::TOTAL) {
            /** DO NOTHING */
        }
    }

    unsigned int mOptions;
    const std::vector<unsigned int>& mVTypes;
};


#endif	/* SMARTCUTAPPLICATOR_HPP */

