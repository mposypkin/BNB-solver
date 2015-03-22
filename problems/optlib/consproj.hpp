/* 
 * File:   consproj.hpp
 * Author: mikdebian
 *
 * Adaptive constraint projection method
 * Created on March 24, 2013, 10:49 AM
 */

#ifndef CONSPROJ_HPP
#define	CONSPROJ_HPP

#include <stdio.h>
#include <limits>
#include <problems/optlib/objective.hpp>
#include <problems/optlib/locoptim.hpp>
#include <util/common/smartptr.hpp>
#include <util/common/vec.hpp>
#include <util/box/boxutils.hpp>
#include <util/common/bnblog.hpp>

template <class FT, int LOG = BNB_NO_LOGGING> class ConsProj : public LocalOptimizer<FT> {
public:

    struct Options {
        /**
         * The initial value of 'a'
         */
        FT mInitA;

        /**
         * The initial value of 'b'
         */
        FT mInitB;

        /**
         * Increment multiplier for 'a'
         */
        FT mIncA;

        /**
         * Increment multiplier for 'b'
         */
        FT mIncB;

        /**
         * Decrement multiplier for 'a'
         */
        FT mDecA;

        /**
         * Decrement multiplier for 'b'
         */
        FT mDecB;

        /**
         * Upper bound for iterations
         */
        FT mMaxSteps;

        /**
         * The minimal difference on successive values
         */
        FT mEps;

        /**
         * The minimal norm on constraint gradient projection
         */
        FT mDelta;

        /**
         * The minimal value of 'a'
         */
        FT mMinA;

        /**
         * The minimal value of 'b'
         */
        FT mMinB;

        Options() :
        mInitA(1.0), mInitB(1.0), mIncA(1.1), mIncB(1.1), mDecA(0.5), mDecB(0.5),
        mMaxSteps(std::numeric_limits<int>::max()), mEps(0.001), mDelta(0.001),
        mMinA(0.0000000001), mMinB(0.0000000001) {
        }
    };

    /** 
     * Constructor
     * @param obj objective
     * @param cons constraint
     * @param box bounding box
     * @param eps tolerance on the objective value change
     */
    ConsProj(Objective<FT> *obj, Objective<FT> *cons, Box<FT>* box) : mObj(obj), mCons(cons), mBox(box){
    }

    /**
     * Perform search
     * @param x start point and result
     * @param v 
     * @return true if search converged and false otherwise
     */
    bool search(FT* x, FT* v) {
        int n = mObj->getDim();
        FT a = mOpt.mInitA;
        FT b = mOpt.mInitB;
        FT vf;
        FT vold = std::numeric_limits<FT>::max();
        FT norm;
        SmartArrayPtr<FT> grobj;
        SmartArrayPtr<FT> gcons;
        SmartArrayPtr<FT> gconsproj;
        SmartArrayPtr<FT> y;
        SmartArrayPtr<FT> z;
        grobj.alloc(n);
        gcons.alloc(n);
        gconsproj.alloc(n);
        y.alloc(n);
        z.alloc(n);
        int i = 0;
        while (true) {
            FT vc;
            i++;
            //mProj->project(xnew);
            vf = mObj->func(x);
            //FT vcn = mCons->func(x);
            mLog << "step " << i << ", f = " << vf << ", constr = " << vc;
            mLog << ", a = " << a << ", b = " << b << "\n";
            mLog.write();

            if (i > mOpt.mMaxSteps) {
                mLog << "Performed more than " << mOpt.mMaxSteps << " steps \n";
                mLog.write();
                break;
            }
            if (BNBABS(vf - vold) < mOpt.mEps) {
                mLog << "Progress less that eps = " << mOpt.mEps << "\n";
                mLog.write();
                break;
            }
            vold = vf;
            //VecUtils::vecPrint(n, x);
            mObj->grad(x, grobj);
            mCons->grad(x, gcons);
            VecUtils::project(n, (FT*) grobj, (FT*) gcons, (FT*) gconsproj);
            norm = VecUtils::vecNormTwo(n, (FT*) gconsproj);
            if (norm < mOpt.mDelta) {
                mLog << " norm is below " << mOpt.mDelta << "\n";
                mLog.write();
                break;
            }
            mLog << "norm = " << norm << ", a = " << a << ", b = " << b << "\n";
            mLog.write();
            a *= mOpt.mIncA;
            b *= mOpt.mIncB;
            while (true) {
                VecUtils::vecSaxpy(n, x, (FT*) gconsproj, -a, (FT*) y);
                //BoxUtils::project((FT*) y, *mBox);
                if (LOG == BNB_DO_LOGGING)
                    VecUtils::vecPrint(n, (FT*) y);
                if ((!BoxUtils::isIn((FT*) y, *mBox)) || ((vc = mCons->func(y)) > 0)) {
                    a *= mOpt.mDecA;
                    if (a < mOpt.mMinA) {
                        mLog << "a drops below limit\n";
                        mLog.write();
                        return false;
                    }
                    mLog << "vc = " << vc << ", a reduced to " << a;
                    mLog << (BoxUtils::isIn((FT*) y, *mBox) ? "(in)" : "(out)") << "\n";
                    mLog.write();
                    continue;
                } else
                    break;
            }
            while (true) {
                VecUtils::vecSaxpy(n, (FT*) y, (FT*) grobj, -b, (FT*) z);
                //BoxUtils::project((FT*) z, *mBox);
                if ((!BoxUtils::isIn((FT*) z, *mBox)) || ((vc = mCons->func(z)) > 0)) {
                    b *= mOpt.mDecB;
                    if (b < mOpt.mMinB) {
                        mLog << "b drops below limit\n";
                        mLog.write();
                        return false;
                    }
                    mLog << "vc = " << vc << ", b reduced to " << b;
                    mLog << (BoxUtils::isIn((FT*) z, *mBox) ? "(in)" : "(out)") << "\n";
                    mLog.write();
                    mLog.write();
                } else
                    break;
            }
            VecUtils::vecCopy(n, (FT*) z, x);
        }
        *v = vf;
        return true;
    }

    Options& getOptions() {
        return mOpt;
    }

private:
    Objective<FT> *mCons;
    Objective<FT> *mObj;
    Box<FT> *mBox;
    BNBLog <LOG> mLog;
    Options mOpt;
};


#endif	/* CONSPROJ_HPP */

