/**
 * File:   boxconbnb.hpp
 * Author: mikdebian
 * 
 * Classical Branch-and-Bound framework 
 * for box-constrained problems
 *
 * Created on April 15, 2013, 4:24 AM
 */

#ifndef BOXCONBNB_HPP
#define	BOXCONBNB_HPP

#include <vector>
#include <set>
#include <limits>
#include <functional>

#include <kernel/dmmemory/bnbresolver.hpp>
#include <problems/optlib/locoptim.hpp>
#include <problems/boxcon/common/boxconproblem.hpp>
#include <util/box/boxutils.hpp>
#include <util/box/box.hpp>
#include <util/common/bnblog.hpp>
#include <util/common/smartptr.hpp>

/**
 * The box-constrained classical B&B solver. Boxes are discarded only if lower bound is 
 * larger than incumbent - eps.
 */
template <typename FT, int LOG = BNB_NO_LOGGING> class BoxconBNB : public BNBResolver {
public:

    /**
     * Options for tuning the resolver
     */
    struct Options {

        Options() : mEps(0.1) {
        }
        /**
         * Tolerance to decide whether to put a subproblem or to skip it
         * the problem is put if lb less than incumbent - eps.
         * 10^-1 by default. 
         */
        FT mEps;
    };

    struct Solution {

        Solution() : mValue(std::numeric_limits<FT>::max()) {
        }
        FT mValue;
        SmartArrayPtr<FT> mX;
    };

    struct Sub {

        Sub(Box<FT>& box) : mBox(box), mBound(std::numeric_limits<FT>::max()) {
        }
        Box<FT> mBox;
        FT mBound;
    };

    class Cmp {
    public:

        bool operator() (const Sub& s1, const Sub& s2) const {
            return (s1.mBound < s2.mBound);
        }
    };

    /**
     * Abstract class for bounder
     */
    class Bounder {
    public:
        /**
         * Compute the upper (incumbent) and lower (sub.mBound) bounds
         */
        virtual void bound(Sub& sub, Solution& incumbent) = 0;
    };

    BoxconBNB() : mCurBounder(0), mHandler([](Solution& sol, Sub& sub) {}) {
        mIncumbent.mValue = std::numeric_limits<FT>::max();
    }

    void init(BoxconProblem<FT>& bcp) {
        typename BoxconBNB<FT>::Sub sub(bcp.mBox);
        setInitialSub(sub);
    }

    /**
     * Adds bounder
     */
    void addBounder(Bounder* bnd) {
        mBounders.push_back(bnd);
    }

    /**
     * Setup handler to call at each iteration (incumbent and current subproblem are passed)
     * @param hnd
     */
    void setHandler(std::function<void (Solution& sol, Sub& sub)> hnd) {
        mHandler = hnd;
    }
    
    /**
     * Initialize the list of subproblems
     * @param sub
     */
    void setInitialSub(Sub& sub) {
        putSub(sub);
    }

    /**
     * Get incumbent
     * @return 
     */
    Solution& getIncumbent() {
        return mIncumbent;
    }

    /**
     * Perform several algorithmic steps (or less)
     * @param steps on entry: requested number of steps, on exit: actual number of steps
     *        -1 on exit means an error
     */
    void solve(long long& steps) {
        int count = -1;
        while (true) {
            count++;
            if (count >= steps)
                break;
            if (mSubs.empty()) {
                steps = count;
                break;
            }
            Sub s = getSub();
            mHandler(mIncumbent, s);
            int n = s.mBox.mDim;
            Box<FT> lb(n), rb(n);
            BoxUtils::divideByLongestEdge(s.mBox, lb, rb);
            Sub ls(lb);
            Sub rs(rb);
            putSub(ls);
            putSub(rs);

        }
    }

    Options& getOptions() {
        return mOptions;
    }

private:

    Sub getSub() {
        typename std::multiset< Sub, Cmp>::iterator i = mSubs.begin();
        Sub s = *i;
        mSubs.erase(i);
        return s;
    }

    void putSub(Sub & s) {
        BNB_ASSERT(mBounders.size() > mCurBounder);
        mBounders[mCurBounder]->bound(s, mIncumbent);
        // TMP SOLUTION
#if 0        
        const FT R = 5;
        FT r = BoxUtils::radius(s.mBox);
        if(r < R)
            return;
#endif        
        // TMP
        if (s.mBound < mIncumbent.mValue - mOptions.mEps)
            mSubs.insert(s);
    }

    std::multiset< Sub, Cmp > mSubs;
    Solution mIncumbent;
    BNBLog <LOG> mLog;
    std::vector< Bounder* > mBounders;
    std::function< void (Solution& sol, Sub& sub) > mHandler;
    int mCurBounder;
    Options mOptions;
};


#endif	/* BOXCONBNB_HPP */

