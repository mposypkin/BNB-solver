#ifndef __BOXUTILS_HPP__
#define __BOXUTILS_HPP__
/**
 * Cut a ball from a box
 */

#include <math.h>
#include <sstream>
#include <iostream>
#include <vector>
#include "util/common/utilmacro.hpp"
#include "box.hpp"
#include "util/common/segment.hpp"

class BoxUtils {
public:

    /**
     * Calculated box center
     * @param box under consideration
     * @param c center
     */
    template <class FT> static void getCenter(const Box<FT> &box, FT* c) {
        int n = box.mDim;
        FT* a = box.mA;
        FT* b = box.mB;
        for (int i = 0; i < n; i++) {
            c[i] = 0.5 * (a[i] + b[i]);
        }

    }

    /**
     * Returns the box radius
     * @param box under consideration
     * @return box radius
     */
    template <class FT> static FT radius(const Box<FT> &box) {
        FT dq = 0.;
        int n = box.mDim;
        FT* a = box.mA;
        FT* b = box.mB;
        for (int i = 0; i < n; i++) {
            dq += BNBSQR(b[i] - a[i]);
        }
        return 0.5 * sqrt(dq);
    }

    /**
     * Cuts a (L2) ball with center c and radius r from rectangle
     * @param s split dimension
     * @param box box to split
     * @param c ball center
     * @param rq ball (square) radius
     * @param seg interval to extract accross split dimension
     */
    template <class FT> static bool cutL2(int s, const Box<FT> &box, const FT* c, FT rq, Segment<FT>& seg) {
        FT dq = 0.;
        int n = box.mDim;
        FT* a = box.mA;
        FT* b = box.mB;
        for (int i = 0; i < n; i++) {
            if (i != s) {
                FT di = BNBMAX(BNBABS(c[i] - a[i]), BNBABS(c[i] - b[i]));
                dq += di * di;
            }
        }
        if (dq >= rq)
            return false;
        else {
            double delt = sqrt(rq - dq);
            seg.mA = c[s] - delt;
            seg.mB = c[s] + delt;
            if ((seg.mA >= b[s]) || (seg.mB <= a[s]))
                return false;
            else {
                //printf("{%lf, %lf, %e} %s", seg.mB, a[s], seg.mB - a[s], (seg.mB > a[s]) ? "true" : "false");
                return true;
            }
        }
    }

    /**
     * Retrieves the number of the longest edge from the box
     * @param box box to process
     * @return number of longest edge
     */
    template <class FT> static int getLongestEdge(const Box<FT> &box) {
        int I = 0;
        int n = box.mDim;
        FT e = box.mB[0] - box.mA[0];
        for (int i = 1; i < n; i++) {
            FT ei = box.mB[i] - box.mA[i];
            if (ei > e) {
                e = ei;
                I = i;
            }
        }
        return I;
    }

    /**
     * Split the box into two equal boxes along the longest edge
     * @param box initial box
     * @param boxl left box
     * @param boxr right box
     */
    template <class FT> static void divideByLongestEdge(const Box<FT> & box, Box<FT> &boxl, Box<FT> &boxr) {
        int n = box.mDim;
        int I = getLongestEdge(box);
        for (int i = 0; i < n; i++) {
            boxl.mA[i] = boxr.mA[i] = box.mA[i];
            boxl.mB[i] = boxr.mB[i] = box.mB[i];
        }
        FT c = 0.5 * (box.mA[I] + box.mB[I]);
        boxl.mB[I] = boxr.mA[I] = c;
    }

    /**
     * Returns the stringified representation of the box
     * @param box box to print
     * @return string result
     */
    template <class FT> static std::string toString(const Box<FT> &box) {
        std::ostringstream os;
        int n = box.mDim;
        for (int i = 0; i < n; i++) {
            if (i != 0) os << "x";
            os << "[" << box.mA[i] << "," << box.mB[i] << "]";
        }
        return os.str();
    }

    /**
     * Checks whether the point is within the box
     * @param x the point to check
     * @param box the box to check
     * @return true if the point is in, false otherwise
     */
    template <class FT> static bool isIn(const FT* x, const Box<FT> &box) {
        int n = box.mDim;
        bool rv = true;
        for (int i = 0; i < n; i++) {
            if (x[i] > box.mB[i]) {
                rv = false;
                break;
            }
            if (x[i] < box.mA[i]) {
                rv = false;
                break;
            }
        }
        return rv;
    }

    /**
     * Checks whether box bin lies inside bout
     * @param bout outer box
     * @param bin inner box
     * @return true in bin is a subbox of bout
     */
    template <class FT> static bool isSubBox(const Box<FT> &bout, const Box<FT> &bin) {
        int n = bin.mDim;
        bool rv = true;
        for (int i = 0; i < n; i++) {
            if (bin.mA[i] < bout.mA[i]) {
                rv = false;
                break;
            }
            if (bin.mB[i] > bout.mB[i]) {
                rv = false;
                break;
            }
        }
        return rv;
    }

    /**
     * Checks whether box bin lies strictly inside bout
     * (no common border points)
     * @param bout outer box
     * @param bin inner box
     * @return true in bin is a subbox of bout
     */
    template <class FT> static bool isStrictSubBox(const Box<FT> &bout, const Box<FT> &bin) {
        int n = bin.mDim;
        bool rv = true;
        for (int i = 0; i < n; i++) {
            if (bin.mA[i] <= bout.mA[i]) {
                rv = false;
                break;
            }
            if (bin.mB[i] >= bout.mB[i]) {
                rv = false;
                break;
            }
        }
        return rv;
    }

    /**
     * Checks whether two box identical
     * @param bone first box
     * @param btwo second box
     * @return true in boxes are same
     */
    template <class FT> static bool isSameBox(const Box<FT> &bone, const Box<FT> &btwo) {
        int n = bone.mDim;
        BNB_ASSERT(n == btwo.mDim);
        bool rv = true;
        for (int i = 0; i < n; i++) {
            if (bone.mA[i] != btwo.mA[i]) {
                rv = false;
                break;
            }
            if (bone.mB[i] != btwo.mB[i]) {
                rv = false;
                break;
            }
        }
        return rv;
    }

    /**
     * Computes the projection of a point to a box 
     * @param x point to project
     * @param box box to project to
     */
    template <class FT> static void project(FT* x, const Box<FT> &box) {
        int n = box.mDim;
        for (int i = 0; i < n; i++) {
            if (x[i] > box.mB[i])
                x[i] = box.mB[i];
            else if (x[i] < box.mA[i])
                x[i] = box.mA[i];
        }
    }

    /**
     * Copies box boundaries (must be of the same dimension)
     * @param sbox source box
     * @param dbox dest box
     */
    template <class FT> static void copy(const Box<FT> &sbox, Box<FT> &dbox) {
        int n = sbox.mDim;
        BNB_ASSERT(n == dbox.mDim);
        for (int i = 0; i < n; i++) {
            dbox.mB[i] = sbox.mB[i];
            dbox.mA[i] = sbox.mA[i];
        }
    }

    /**
     * Compute complement (list of boxes) of a box in a box
     * @param sbox source box 
     * @param cbox complementary box
     * @param result resulting list of boxes (list = sbox \ cbox)
     */
    template <class FT> static void complement(const Box<FT> &sbox, const Box<FT> &cbox, std::vector< Box<FT> >& result) {
        int n = sbox.mDim;
        BNB_ASSERT(cbox.mDim == n);
        Box<FT> box(n);
        
        copy(sbox, box);
        bool intersect = true;
        for (int i = 0; i < n; i++) {
            if ((cbox.mB[i] < box.mA[i]) || (cbox.mA[i] > box.mB[i])) {
                intersect = false;
            }
        }
        if (intersect) {
            for (int i = 0; i < n; i++) {

                if (cbox.mA[i] > box.mA[i]) {
                    Box<FT> nbox(n);
                    copy(box, nbox);
                    nbox.mB[i] = cbox.mA[i];
                    box.mA[i] = cbox.mA[i];
                    result.push_back(nbox);
                }
                if (cbox.mB[i] < box.mB[i]) {
                    Box<FT> nbox(n);
                    copy(box, nbox);
                    nbox.mA[i] = cbox.mB[i];
                    box.mB[i] = cbox.mB[i];
                    result.push_back(nbox);
                }

            }
        } else {
            result.push_back(box);
        }
    }

    /**
     * Compute intersection of 2 boxes
     * @param fbox 1st box
     * @param sbox 2nd box
     * @param result resulting box
     * @return true if there is intersection
     */
    template <class FT> static bool intersect(const Box<FT> &fbox, const Box<FT> &sbox, Box<FT>& result) {
        int n = fbox.mDim;
        bool rv;
        BNB_ASSERT(sbox.mDim == n);
        BNB_ASSERT(result.mDim == n);
        for (int i = 0; i < n; i++) {
            result.mA[i] = BNBMAX(fbox.mA[i], sbox.mA[i]);
            result.mB[i] = BNBMIN(fbox.mB[i], sbox.mB[i]);
            if (result.mA[i] > result.mB[i]) {
                rv = false;
                break;
            }
        }
        return rv;
    }

    /**
     * Compute volume 
     * @param box box
     * @return  computed volume
     */
    template <class FT> static FT volume(const Box<FT> &box) {
        int n = box.mDim;
        FT v = 1;
        for (int i = 0; i < n; i++) {
            v *= box.mB[i] - box.mA[i];
        }
        return v;
    }
};
#endif

