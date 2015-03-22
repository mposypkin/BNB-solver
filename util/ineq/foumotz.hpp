/* 
 * File:   foumotz.hpp
 * Author: posypkin
 *
 * Fourier-Motzkin elimination for solving linear inequalities
 *
 * Created on April 24, 2011, 9:39 PM
 */

#ifndef FOUMOTZ_HPP
#define	FOUMOTZ_HPP

#include "util/common/util.hpp"
#include "ineqsolver.hpp"

template <typename FT> class FourierMotzkin : public IneqSolver<FT> {
public:

    struct Sorts {
        /**
         * Normal inequality
         */
        static const int NORMAL = -1;
        /**
         * Eliminated normal inequality
         */
        static const int CLOSED = -2;
    };

    struct Signs {
        /**
         * Less or equal
         */
        static const int LEQ = 1;

        /**
         * Great or equal
         */
        static const int GEQ = -1;

    };

public:

    struct Options {
        /**
         * Check strict inequalities
         */
        const static unsigned int USE_STRICT = 1;
    };

    /**
     * Constructor
     * @param nv number of variables
     * @param nineq number of inequalities
     * @param data inequality coefficients (nv + 1) x nineq array (the last is constant)
     */
    void setInequalities(int nv, int nineq, FT* data) {
        BNB_ASSERT(nv < MAX_VARS);
        BNB_ASSERT(nineq < MAX_INEQS);
        mOptions = Options::USE_STRICT;
        mNV = nv;
        mNIneq = nineq;
        for (int i = 0; i < MAX_INEQS; i++) {
            mIneq[i].mSort = Sorts::NORMAL;
            mIneq[i].mSign = Signs::LEQ;
            mIneq[i].mNV = nv;
            for (int j = 0; j < (MAX_VARS + 1); j++)
                mIneq[i].mCoeff[j] = 0;
        }
        for (int i = 0; i < nineq; i++) {
            for (int j = 0; j < (nv + 1); j++) {
                mIneq[i].mCoeff[j] = data[i * (nv + 1) + j];
            }
        }

        //print(mNIneq);
    }

    /**
     * Get options
     * @return options reference
     */
    unsigned int& getOptions() {
        return mOptions;
    }

    /**
     * Finds a feasible direction according to the inequalities
     * @param p the found vector
     * @return true if such vector exists false otherwise
     */
    bool findFeasible(FT* p) {
        int end = mNIneq;
        int cur = mNIneq;
        for (int i = 0; i < mNV; i++) {
            for (int j = 0; j < end; j++) {
                if (mIneq[j].mSort == Sorts::NORMAL) {
                    if (mIneq[j].mCoeff[i] != 0) {
                        FT a = -mIneq[j].mCoeff[i];
                        mIneq[j].mSort = Sorts::CLOSED;
                        mIneq[cur].mSort = i;
                        for (int k = 0; k < (mNV + 1); k++) {
                            mIneq[cur].mCoeff[k] = mIneq[j].mCoeff[k] / a;
                        }
                        mIneq[cur].mCoeff[i] = 0;
                        /*mIneq[cur].mSign = (a > 0) ? Signs::GEQ : Signs::LEQ;*/
                        if(a > 0)
                            mIneq[cur].mSign = Signs::GEQ;
                        else
                            mIneq[cur].mSign = Signs::LEQ;
                        cur++;
                    }
                }
            }
            end = cur;
            for (int j = end - 1;; j--) {
                if ((mIneq[j].mSort < 0) || (mIneq[j].mSort != i))
                    break;
                else if (mIneq[j].mSign == Signs::GEQ) {
                    for (int k = end - 1;; k--) {
                        if ((mIneq[k].mSort < 0) || (mIneq[k].mSort != i))
                            break;
                        else if (mIneq[k].mSign == Signs::LEQ) {
                            mIneq[cur].mSign = Signs::LEQ;
                            mIneq[cur].mSort = Sorts::NORMAL;
                            for (int l = 0; l < (mNV + 1); l++) {
                                mIneq[cur].mCoeff[l] = mIneq[j].mCoeff[l] - mIneq[k].mCoeff[l];
                            }
                            cur++;
                        }
                    }
                }
            }
            end = cur;
            /*
            printf("=========== step %d:\n", i);
            print(end);
             */
        }
        //printf("++++++++++++++++++++\n");
        int k = mNV - 1;
        FT l = getMinValue<FT > (), u = getMaxValue<FT > ();
        fillComponent(p + k, l, u);
        for (int i = (end - 1); i >= 0; i--) {
            if (mIneq[i].mSort == Sorts::NORMAL) {
                if (!checkFeasible(mIneq[i]))
                    return false;
                l = getMinValue<FT > ();
                u = getMaxValue<FT > ();
            } else if (mIneq[i].mSort >= 0) {
                if (k != mIneq[i].mSort) {
                    l = getMinValue<FT > ();
                    u = getMaxValue<FT > ();
                    for (int t = k - 1; t > mIneq[i].mSort; t--) {
                        fillComponent(p + t, l, u);
                    }
                    k = mIneq[i].mSort;
                }
                if (mIneq[i].mSign == Signs::LEQ) {
                    u = BNBMIN(u, getValue(mIneq[i], p));
                } else if (mIneq[i].mSign == Signs::GEQ) {
                    l = BNBMAX(l, getValue(mIneq[i], p));
                }
                fillComponent(p + k, l, u);
                /*
                printf("k = %d, l = %lf, u = %lf\n", k, l, u);
                printf("p[%d] = %lf\n", k, p[k]);
                 */
            }
        }
        return true;
    }

private:

    static const int MAX_VARS = 8;

    static const int MAX_INEQS = 128;

    static const FT SHIFT;

    unsigned int mOptions;

    struct Inequality {
        int mSort;
        int mSign;
        int mNV;
        FT mCoeff[MAX_VARS + 1];

        std::string toString() const {
            std::ostringstream os;
            os << "(" << mSort << "," << mSign << ") ";
            for (int i = 0; i < (mNV + 1); i++) {
                os << mCoeff[i] << " ";
            }
            return os.str();
        }
    };

    int mNV;

    int mNIneq;

    Inequality mIneq[MAX_INEQS];

    bool checkFeasible(const Inequality & ineq) const {
        BNB_ASSERT(ineq.mSort == Sorts::NORMAL);
        for (int i = 0; i < mNV; i++) {
            if (ineq.mCoeff[i] != 0) {
                return true;
            }
        }

        if (mOptions & Options::USE_STRICT) {
            if (ineq.mCoeff[mNV] >= 0)
                return false;
            else
                return true;
        } else {
            if (ineq.mCoeff[mNV] > 0)
                return false;
            else
                return true;
        }
    }

    FT getValue(const Inequality & ineq, FT *p) const {
        BNB_ASSERT(ineq.mSort >= 0);
        FT v = ineq.mCoeff[mNV];
        for (int i = (mNV - 1); i > ineq.mSort; i--) {
            v += ineq.mCoeff[i] * p[i];
        }
        return v;
    }

    void print(int mineq) {
        for (int i = 0; i < mineq; i++) {
            printf("%s\n", mIneq[i].toString().c_str());
        }
    }

    void fillComponent(FT* c, FT l, FT u) {
        if ((l == getMinValue<FT > ()) && (u == getMaxValue<FT > ())) {
            *c = 1.;
        } else if (l == getMinValue<FT > ()) {
            *c = u - SHIFT;
        } else if (u == getMaxValue<FT > ()) {
            *c = l + SHIFT;
        } else {
            *c = (l + u) / 2;
        }
    }

};

template<typename FT>
const FT FourierMotzkin<FT>::SHIFT = 1;



#endif	/* FOUMOTZ_HPP */

