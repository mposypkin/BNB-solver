/* 
 * File:   mboxlocsearch.hpp
 * Author: posypkin
 *
 * Local search for multiobjective optimization with box constraints
 *
 * Created on April 24, 2011, 1:50 AM
 */

#ifndef MBOXLOCSEARCH_HPP
#define	MBOXLOCSEARCH_HPP

#include <problems/optlib/locboxoptim.hpp>
#include "multilocsearch.hpp"
#include "convobjective.hpp"


template <typename FT> class MBoxLocalSearch : public MultiLocalSearch<FT> {
public:

    struct Options {
        /**
         * Shake coefficients befor next search
         */
        static const unsigned int SHAKE_WEIGHTS = 1;
    };

    /**
     * The constructor
     * @param problem data
     */
    MBoxLocalSearch(MultiData<FT> &md, std::vector<FT>& coeff, LocalBoxOptimizer<FT>* opt) :
    MultiLocalSearch<FT>(md),
    mObj(md.mCriteria, coeff),
    mBox(md.mBox),
    mOptions(0),
    mOpt(opt) {
        mOpt->setObjective(&mObj);
        BNB_ASSERT(md.mConstraints.empty());
    }

    /**
     * Performs a local search from the given point
     * @param mp starting point
     */
    void search(MultiPoint<FT>& mp) {
        FT v;
        int m = mp.mCrit.mDim;
        if (mOptions & Options::SHAKE_WEIGHTS)
            for (int i = 0; i < m; i++) {
                mObj.getCoeff()[i] = (FT) rand() / (FT) RAND_MAX;
            }
        mOpt->search((FT*) (mp.mParam.mX), &v);
        for (int i = 0; i < m; i++) {
            mp.mCrit.mX[i] = MultiLocalSearch<FT>::mMD.mCriteria[i]->func(mp.mParam.mX);
        }
    }

    /**
     * Retrieve options
     * @return options reference
     */
    unsigned int& getOptions() {
        return mOptions;
    }

private:

    unsigned int mOptions;
    Box<FT> mBox;
    ConvolObjective<FT> mObj;
    LocalBoxOptimizer<FT>* mOpt;
};



#endif	/* MBOXLOCSEARCH_HPP */

