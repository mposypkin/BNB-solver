/* 
 * 
 * Gnanular radial search proposed by D. Powell
 * 
 * File:   granradsearch.hpp
 * Author: mikdebian
 *
 * Created on November 15, 2013, 11:38 AM
 */

#ifndef GRANRADSEARCH_HPP
#define	GRANRADSEARCH_HPP

#include <util/common/vec.hpp>
#include <util/common/bnberrcheck.hpp>
#include <util/common/smartptr.hpp>
#include "locboxoptim.hpp"

/**
 * Maximal size of statistical vector
 */
#define STAT_MAX_SIZE 10 * 1024

template <class FT> class GranRadSearch : public LocalBoxOptimizer <FT> {
public:

    /**
     * Determines stopping conditions
     */
    class Stopper {
    public:
        /**
         * Returns true if it is time to stop
         * @param moves total number of moves
         * @param fval objective value
         * @param gran granular size
         * @return true if it's time to stop
         */
        virtual bool stopnow(int moves, FT fval, FT gran) = 0;
    };

    /**
     * Determines properties of the search method
     */
    struct Properties {
        /**
         * Size of statistics record
         */
        int mStatSize;
        
        /**
         * Treshold to decrease grain
         */
        double mTresh;
        
        /**
         * Initial Grain 
         */
        double mInitGrain;
        
        /**
         * Grain decrease factor
         */
        double mGrainDec;

    };

    GranRadSearch(Box<FT>& box) : LocalBoxOptimizer <FT> (box) {
        setupDefaultProperties();
    }

    /**
     * Perform search
     * @param x start point and result
     * @param v pointer to the resulting value
     * @return true if search converged and false otherwise
     */
    bool search(FT* x, FT* v) {
        bool rv;
        BNB_ASSERT(mStopper);
        Objective<FT>* obj = LocalOptimizer<FT>::getObjective();
        resetStats();
        int moves = 0;


        project(x);
        FT uold = obj->func(x);
        FT u;
        FT g = mProperties.mInitGrain;
        int i = 0;
        int n = obj->getDim();
        while (!mStopper->stopnow(moves, uold, g)) {
            bool goodmove = false;
            int vn = BNBMIN(n-1, (int) (((double) n) * ((double) rand() / (double) RAND_MAX)));
            //printf("vn = %d\n", vn);
            double r = 0.5 - ((double) rand() / (double) RAND_MAX);
            double t = r * g * x[vn];
            double xvn = x[vn];
            x[vn] += t;
            project(x);
            u = obj->func(x);
            if (u < uold) {
                uold = u;
                goodmove = true;
            } else {
                x[vn] = xvn;
            }
            updateStats(goodmove);
            moves++;
            if(mGoodBadRatio <= mProperties.mTresh) {
                g *= mProperties.mGrainDec;
                //printf("move %d: decreased g to %lf, success rate %lf, incumbent %f\n", moves, g, mGoodBadRatio, uold);
                resetStats();
            }
        }
        *v = uold;
        return true;
    }

    /**
     * Setup stopper
     * @param stopper
     */
    void setStopper(Stopper* stopper) {
        mStopper = stopper;
    }
    
    /**
     * Returns properties
     * @return reference to properties
     */
    Properties& getProperties() {
        return mProperties;
    }

private:

    void setupDefaultProperties() {
        mProperties.mStatSize = 500;
        mProperties.mInitGrain = 0.5;
        mProperties.mGrainDec = 0.5;
        mProperties.mTresh = 0.005;       
    }
    
    void updateStats(bool goodmove) {
        if (mStatRecord[mPoint] == 1)
            mGoodMoves--;
        else if (mStatRecord[mPoint] == -1)
            mBadMoves--;

        if (goodmove) {
            mStatRecord[mPoint] = 1;
            mGoodMoves++;
        } else {
            mStatRecord[mPoint] = -1;
            mBadMoves++;
        }
        mPoint = (mPoint + 1) % mProperties.mStatSize;
        if (mStatRecord[mPoint] != 0)
            mGoodBadRatio = (double) mGoodMoves / (double) mBadMoves;
    }

    void project(FT* x) {
        int n = LocalOptimizer<FT>::getObjective()->getDim();
        for (int i = 0; i < n; i++) {
            if (x[i] < LocalBoxOptimizer<FT>::mBox.mA[i])
                x[i] = LocalBoxOptimizer<FT>::mBox.mA[i];
            else if (x[i] > LocalBoxOptimizer<FT>::mBox.mB[i])
                x[i] = LocalBoxOptimizer<FT>::mBox.mB[i];
        }
    }

    void resetStats() {
        mGoodMoves = 0;
        mBadMoves = 0;
        mPoint = 0;
        mGoodBadRatio = 1;
        for (int i = 0; i < mProperties.mStatSize; i++)
            mStatRecord[i] = 0;
    }

    Stopper *mStopper;
    Properties mProperties;
    int mGoodMoves;
    int mBadMoves;
    int mPoint;
    double mGoodBadRatio;
    char mStatRecord[STAT_MAX_SIZE];
};
#endif	/* GRANRADSEARCH_HPP */

