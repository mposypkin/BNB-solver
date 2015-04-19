/* 
 * File:   cututils.hpp
 * Author: medved
 *
 * Created on November 2, 2014, 8:41 PM
 */

#ifndef CUTUTILS_HPP
#define	CUTUTILS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <util/box/boxutils.hpp>
#include "cut.hpp"

template <class FT> class CutUtils {
public:

    /**
     * Produce a string representation of a cut
     * @param cut cut to print
     * @return the string representation
     */
    static std::string toString(const Cut<FT>& cut) {
        std::ostringstream os;
        os << "Cut type " << cut.mType << ", R = " << cut.mR;
        int n = cut.mC.size();
        os << ", centre = [";
        for (int i = 0; i < n; i++) {
            if (i != 0)
                os << ",";
            os << cut.mC[i];
        }
        os << "]";
        return os.str();
    }

#if 0

    /**
     * Appies the given cut to a box
     * @param cut to apply
     * @param box box (to which cuts are applied)
     * @param v resulting vector of boxes
     */
    static void ApplyCut(const Cut<FT>& cut, const Box<FT>& box, std::vector< Box<FT> >& v) {
        if (cut.mType == Cut<FT>::CutType::INNER_BALL) {
            ApplyInnerBallCutSimple(cut, box, v);
        } else if (cut.mType == Cut<FT>::CutType::OUTER_BALL) {
            ApplyOuterBallCut(cut, box, v);
        } else if (cut.mType == Cut<FT>::CutType::LINEAR) {
            ApplyLinearCut(cut, box, v);
        } else if (cut.mType == Cut<FT>::CutType::TOTAL) {
            /** DO NOTHING */
        }
    }
#endif

    /**
     * Checks if the box is inside the cut
     * @param cut cut
     * @param box box
     * @return true is a box in completely inside a cut, false otherwise
     */
    static bool isIn(const Cut<FT>& cut, const Box<FT>& box) {
        int n = box.mDim;
        bool rv;
        if (cut.mType == Cut<FT>::CutType::INNER_BALL) {
            FT qm = 0;
            for (int i = 0; i < n; i++) {
                qm += BNBSQR(BNBMAX(BNBABS(cut.mC[i] - box.mA[i]), BNBABS(cut.mC[i] - box.mB[i])));
            }
            if (qm <= cut.mR * cut.mR)
                rv = true;
            else
                rv = false;
        } else if (cut.mType == Cut<FT>::CutType::OUTER_BALL) {
            FT qm = 0;
            for (int i = 0; i < n; i++) {
                if (cut.mC[i] < box.mA[i]) {
                    qm += BNBSQR(box.mA[i] - cut.mC[i]);
                } else if (cut.mC[i] > box.mB[i]) {
                    qm += BNBSQR(cut.mC[i] - box.mB[i]);
                }
            }
            if (qm <= cut.mR * cut.mR)
                rv = false;
            else
                rv = true;
        } else if (cut.mType == Cut<FT>::CutType::LINEAR) {
            FT u = cut.mR;
            for (int i = 0; i < n; i++) {
                u += (cut.mC[i] > 0) ? (cut.mC[i] * box.mA[i]) : (cut.mC[i] * box.mB[i]);
            }
            if (u >= 0)
                rv = true;
            else
                rv = false;
        } else {
            rv = true;
        }
        return rv;
    }

    /**
     * Cuts the ball from a rectangle (simple version)
     * @param cut ball to cut
     * @param box box to apply cut
     * @param v resulting vector
     * @return true if cut makes effect
     */
    static bool ApplyInnerBallCutSimple(const Cut<FT>& cut, const Box<FT>& box, std::vector< Box<FT> >& v) {
        int n = box.mDim;
        FT lmax = 0, amax = 0, bmax = 0;
        int imax;
        for (int i = 0; i < n; i++) {
            FT q = 0;
            for (int j = 0; j < n; j++) {
                if (j != i) {
                    FT p = BNBMAX(BNBABS(box.mA[j] - cut.mC[j]), BNBABS(box.mB[j] - cut.mC[j]));
                    q += p * p;
                }
            }
            FT rq = cut.mR * cut.mR;
            if (q < rq) {
                FT h = sqrt(rq - q);
                FT a = BNBMAX(cut.mC[i] - h, box.mA[i]);
                FT b = BNBMIN(cut.mC[i] + h, box.mB[i]);
                FT l = (b - a) / (box.mB[i] - box.mA[i]);
                //std::cout << "i = " << i << ", l = " << l << ", a = " << a << ", b = " << b << "\n";
                if (l > lmax) {
                    imax = i;
                    lmax = l;
                    amax = a;
                    bmax = b;
                }
            }
        }
        //std::cout << "imax = " << imax << ", lmax = " << lmax << ", amax = " << amax << ", bmax = " << bmax << "\n";
        bool rv;
        if (lmax > 0) {
            if (amax > box.mA[imax]) {
                Box<FT> b(n);
                BoxUtils::copy(box, b);
                b.mB[imax] = amax;
                v.push_back(b);
            }
            if (bmax < box.mB[imax]) {
                Box<FT> b(n);
                BoxUtils::copy(box, b);
                b.mA[imax] = bmax;
                v.push_back(b);
            }
            rv = true;
        } else {
            rv = false;
            v.push_back(box);
        }
        return rv;
    }

    /**
     * Cuts the ball from a rectangle (Boxed based version)
     * @param cut ball to cut
     * @param box box to apply cut
     * @param v resulting vector
     */
    static void ApplyInnerBallCutBoxed(const Cut<FT>& cut, const Box<FT>& box, std::vector< Box<FT> >& v) {
        int n = box.mDim;
        Box<FT> cbox(n);
        double vs = BoxUtils::volume(box);
#if 0        
        double d = cut.mR / sqrt(n);
        for (int i = 0; i < n; i++) {
            cbox.mA[i] = cut.mC[i] - d;
            cbox.mB[i] = cut.mC[i] + d;
        }
        Box<double> isec(n);
        bool rv = BoxUtils::intersect(box, cbox, isec);
        double vc = BoxUtils::volume(isec);
#endif        

        FT vc = FindMaxIntersection(cut, box, cbox);
        
        //std::cout << " vs = " << vs << ", vc = " << vc << "\n";

        /** TMP */
        if ((vc > 0) && (vs / vc < 30))
            BoxUtils::complement(box, cbox, v);
        else
            v.push_back(box);
        /** TMP **/

    }

    /**
     * Find a box inside cut maximizing intersection with the reduced box
     * @param cut spherical cut
     * @param sbox box to reduce
     * @param ibox resulting box
     * @return volume on intersection
     */
    static FT FindMaxIntersection(const Cut<FT>& cut, const Box<FT>& sbox, Box<FT>& ibox) {
        FT v = 0;
        BNB_ASSERT(cut.mType == Cut<FT>::CutType::INNER_BALL);
        int n = sbox.mDim;
        BNB_ASSERT(ibox.mDim == n);
        BNB_ASSERT(cut.mC.size() == n);
        int maxit = 100;
        FT x[n];

        /** Iterate Monte-Carlo **/
        for (int i = 0; i < maxit; i++) {
            FT u = 1;
            FT r = cut.mR;
            /** Compute random subbox and its volume **/
            for (int j = 0; j < n; j++) {
                FT s;
                if (j == n - 1)
                    s = r;
                else
                    s = ((FT(rand()) / (FT) RAND_MAX)) * r;
                FT a = cut.mC[j] - s;
                FT b = cut.mC[j] + s;
                FT ar = BNBMAX(a, sbox.mA[j]);
                FT br = BNBMIN(b, sbox.mB[j]);
                if (ar >= br) {
                    u = -1;
                    break;
                } else
                    u *= br - ar;
                x[j] = s;
                r = sqrt(r * r - s * s);
            }
            if (u > v) {
                v = u;
                for (int j = 0; j < n; j++) {
                    ibox.mA[j] = cut.mC[j] - x[j];
                    ibox.mB[j] = cut.mC[j] + x[j];
                }
            }
        }

        return v;

    }

    /**
     * Apply outer ball cut to a box
     * @param cut cut to apply
     * @param box box to apply cut
     * @param v resulting vector
     */
    static void ApplyOuterBallCut(const Cut<FT>& cut, const Box<FT>& box, std::vector< Box<FT> >& v) {
        int n = box.mDim;
        Box<FT> nbox(n);
        if (isIn(cut, box))
            return;
        for (int i = 0; i < n; i++) {
            FT a = cut.mC[i] - cut.mR;
            nbox.mA[i] = BNBMAX(a, box.mA[i]);
            FT b = cut.mC[i] + cut.mR;
            nbox.mB[i] = BNBMIN(b, box.mB[i]);
            if (nbox.mA[i] > nbox.mB[i]) {
                return;
            }
        }
        v.push_back(nbox);
    }

    /**
     * Applies linear cut to a box
     * @param cut cut to apply
     * @param box box to cut
     * @param v resulting vector
     */
    static void ApplyLinearCut(const Cut<FT>& cut, const Box<FT>& box, std::vector< Box<FT> >& v) {
        int n = box.mDim;
        Box<FT> nbox(n);
        FT u = cut.mR;
        for (int i = 0; i < n; i++) {
            u += (cut.mC[i] > 0) ? (cut.mC[i] * box.mA[i]) : (cut.mC[i] * box.mB[i]);
        }
        if (u >= 0)
            return;

        for (int i = 0; i < n; i++) {
            FT p = cut.mC[i];
            if (p > 0) {
                nbox.mA[i] = box.mA[i];
                nbox.mB[i] = BNBMIN(box.mB[i], box.mA[i] - u / p);
            } else if (p < 0) {
                nbox.mA[i] = BNBMAX(box.mA[i], box.mB[i] - u / p);
                nbox.mB[i] = box.mB[i];
            } else {
                nbox.mA[i] = box.mA[i];
                nbox.mB[i] = box.mB[i];
            }
        }
        v.push_back(nbox);
    }


};

#endif	/* CUTUTILS_HPP */

