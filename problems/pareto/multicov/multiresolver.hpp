/* 
 * File:   multiresolver.hpp
 * Author: posypkin
 *
 * Created on January 5, 2012, 7:46 AM
 */

#ifndef MULTIRESOLVER_HPP
#define	MULTIRESOLVER_HPP

#include <kernel/dmmemory/bnbresolver.hpp>
#include <problems/pareto/multiopt/multiser.hpp>
#include <kernel/dmmemory/sstrat.hpp>
#include <util/boxbnb/boxsub.hpp>
#include <util/boxbnb/boxsubpool.hpp>
#include <util/boxbnb/boxsubser.hpp>
#include "multistate.hpp"
#include "multicov.hpp"

class MultiResolver : public BNBResolver {
public:
    typedef MultiState< MPSimpBag < double >, BoxSubPool<double> > MSType;
    typedef MultiCov<double> MLType;

    MultiResolver(MSType* ms, MLType* ml) : BNBResolver(), mUpdated(false) {
        mMS = ms;
        mML = ml;
    }

    /**
     * Get solver state info
     * @param info information structure to fill
     */
    void getInfo(SolverInfo& info) {
        info.mNSub = mMS->mSubBag.size();
        info.mRecUpdated = mUpdated;
    }

    /**
     * Reset solver state info
     */
    virtual void resetInfo() {
        mUpdated = false;
    }

    /**
     * Push subsets to the serializer
     *
     * @param num number of subsets
     * @param ser the serializer
     */
    void getSubs(int& num, BinarySerializer& ser) {
        int n = BNBMIN(num, mMS->mSubBag.size());
        ser << n;
        for (int i = 0; i < n; i++) {
            BoxSub<double> bx = mMS->mSubBag.get();
            ser << bx;
        }
        num = n;
    }

    /**
     * Push record(s) to the serializer
     * @param set the serilizer
     */
    void getRecords(BinarySerializer& ser) {
        int n = mMS->mPointBag.size();
        ser << n;
        for (int i = 0; i < n; i++) {
            MultiPoint<double> mp = mMS->mPointBag.get();
            ser << mp;
        }
    }

    /**
     * Take subproblems from the serializer and put them to the solver
     *
     * @param ser serializer
     */
    int putSubs(BinarySerializer& ser) {
        int pard = mML->getMultiData().mBox.mDim;
        int n;
        ser >> n;
        for (int i = 0; i < n; i++) {
            BoxSub<double> sub(pard);
            ser >> sub;
            mMS->mSubBag.put(sub);
        }
        return n;
    }

    /**
     * Take record(s) from the serializer and put them to the solver
     *
     * @param ser serializer
     */
    void putRecords(BinarySerializer& ser) {
        int pard = mML->getMultiData().mBox.mDim;
        int critd = mML->getMultiData().mCriteria.size();
        int n;
        ser >> n;
        for (int i = 0; i < n; i++) {
            MultiPoint<double> mp(pard, critd);
            ser >> mp;
            mMS->mPointBag.put(mp);
        }
    }

    /**
     * Setup search strategy
     * @param strategy the search strategy
     */
    void setSearchStrategy(int strategy) {
        if (strategy == SearchStrategies::WFS) {
            mMS->mSubBag.setDirection(BoxSubPool<double>::Directions::TAKE_UPPER);
            ;
        } else if (strategy == SearchStrategies::DFS) {
            mMS->mSubBag.setDirection(BoxSubPool<double>::Directions::TAKE_LOWER);
            ;
        }
    }

    /**
     * Setup heuristic
     * the given heuristic is implementation-specific
     * the bigger number the more complex and hopefully powerful heuristic
     * numbers start from 1 (0 means no heuristic)
     * @param heuristic a heuristic to set
     */
    void setHeuristic(int heuristic) {
        mML->setHeuristic(heuristic);
    }

    /**
     * Setup bounding method
     * the bigger number the more complex and powerful bounding method
     * @param method bounding method to set
     */
    virtual void setBoundingMethod(int method) {
        mML->setBoundingMethod(method);
    }

    /**
     * Perform several algorithmic steps (or less)
     * @param steps on entry: requested number of steps, on exit: actual number of steps
     *        -1 on exit means an error
     */
    void solve(long long& steps) {
        mML->solve(*mMS, steps, &mUpdated);
    }

private:
    MSType* mMS;
    MLType* mML;
    bool mUpdated;
};


#endif	/* MULTIRESOLVER_HPP */

