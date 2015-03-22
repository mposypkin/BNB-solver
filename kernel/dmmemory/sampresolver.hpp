/* 
 * File:   sampresolver.hpp
 * Author: posypkin
 *
 * Sample resolver
 *
 * Created on September 12, 2011, 9:57 AM
 */

#ifndef SAMPRESOLVER_HPP
#define	SAMPRESOLVER_HPP

#include <util/common/segment.hpp>
#include <vector>
#include <util/common/util.hpp>

#include "bnbresolver.hpp"
#include "solverinfo.hpp"

/**
 * Sample resolve that splits an interval into smaller intervals until the lengths
 * of an interval is greater than a given value
 */
class SampleResolver : public BNBResolver {
public:

    /**
     * The constructor
     * @param a the left end
     * @param b the right end
     * @param h the minimal segment size
     */
    SampleResolver(double a, double b, double h) {
        mUpdated = false;
        Segment<double> s;
        s.mA = a;
        s.mB = b;
        mRecord = (a + b) / 2;
        mSegs.push_back(s);
        mH = h;
    }

    /**
     * Get solver state info
     * @param info information structure to fill
     */
    void getInfo(SolverInfo& info) {
        info.mNSub = mSegs.size();
        info.mRecUpdated = mUpdated;
    }

    /**
     * Reset solver state info
     */
    void resetInfo() {
        mUpdated = false;
    }

    /**
     * Push subsets to the serializer
     *
     * @param num number of subsets
     * @param ser the serializer
     */
    void getSubs(int& num, BinarySerializer& ser) {
        int i;
        int u = BNBMIN(num, mSegs.size());
        ser << u;
        for (i = 0; i < u; i++) {
            int sz = mSegs.size();
            ser << mSegs[sz - 1].mA;
            ser << mSegs[sz - 1].mB;
            mSegs.pop_back();
        }
        num = u;
    }

    /**
     * Push record(s) to the serializer
     * @param set the serilizer
     */
    void getRecords(BinarySerializer& ser) {
        ser << mRecord;
    }

    /**
     * Take subproblems from the serializer and put them to the solver
     *
     * @param ser serializer
     * @return number of packed problems
     */
    int putSubs(BinarySerializer& ser) {
        int u;
        ser >> u;
        for (int i = 0; i < u; i++) {
            Segment<double> s;
            ser >> s.mA;
            ser >> s.mB;
            mSegs.push_back(s);
        }
        return u;
    }

    /**
     * Take record(s) from the serializer and put them to the solver
     *
     * @param ser serializer
     */
    void putRecords(BinarySerializer& ser) {
        ser >> mRecord;
    }

    /**
     * Perform several algorithmic steps (or less)
     * @param steps on entry: requested number of steps, on exit: actual number of steps
     *        -1 on exit means an error
     */
    void solve(long long& steps) {
        int i = 0;
        while (true) {
            if(i >= steps)
                break;
            if(mSegs.empty())
                break;
            Segment<double> s = mSegs.back();
            mSegs.pop_back();
            double r = (s.mB + s.mA) / 2;
            if(r < mRecord) {
                mRecord = r;
                mUpdated = true;
            }
            if(s.mB - s.mA > mH) {
                Segment<double> s1;
                Segment<double> s2;
                s1.mA = s.mA;
                s1.mB = r;
                s2.mA = r;
                s2.mB = s.mB;
                mSegs.push_back(s1);
                mSegs.push_back(s2);
            }
            i ++;
        }
        steps = i;
    }

    /**
     * Setup search strategy
     * @param strategy the search strategy
     */
    void setSearchStrategy(int strategy) {
        
    }

private:
    std::vector< Segment<double> > mSegs;
    bool mUpdated;
    double mRecord;
    double mH;
};


#endif	/* SAMPRESOLVER_HPP */

