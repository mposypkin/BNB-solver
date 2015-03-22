#ifndef __POLYLIPOBJ_HPP__
#define __POLYLIPOBJ_HPP__

#include <math.h>
#include <problems/cover/common/lipzobjective.hpp>
#include <problems/poly/polyutil.hpp>
#include <problems/poly/polynomtxt.hpp>
#include <problems/optlib/polyobjective.hpp>

/**
 * Polynomial Lipzic objective
 */

template <typename FT> class PolyLipObjective : public PolyObjective <FT>, public LipzObjective<FT> {
public:

    typedef Polynom<FT> POLYNOM;

    /**
     * Constructor
     * @param poly polynomial
     */
    PolyLipObjective(POLYNOM* poly) : PolyObjective <FT>(poly) {
    }

    FT getFuncLipzConstant(const FT* a, const FT* b) {
        FT gl = 0, gu = 0;
        int n = Objective<FT>::getDim();
        for (int i = 0; i < n; i++) {
            FT l, u;
            PolyObjective <FT>::mGrad[i].getBounds(a, b, &l, &u);
            BNBInterval<FT>::sqr(l, u, &l, &u);
            gl += l;
            gu += u;
        }
        return sqrt(gu);
    }

    void getEigenBounds(const FT* a, const FT* b, FT& emin, FT& emax) {
        int n = Objective<FT>::getDim();
        emin = getMaxValue<FT > ();
        emax = getMinValue<FT > ();
        fillHessBounds(a, b);
        for (int i = 0; i < n; i++) {
            FT gl, gu, r = 0;
            gl = mHessBounds[i][2 * i];
            gu = mHessBounds[i][2 * i + 1];
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    FT l, u;
                    l = mHessBounds[i][2 * j];
                    u = mHessBounds[i][2 * j + 1];
                    FT m = BNBMAX(BNBABS(l), BNBABS(u));
                    r += m;
                }
            }
            gl -= r;
            gu += r;
            emin = BNBMIN(emin, gl);
            emax = BNBMAX(emax, gu);
        }
    }

    FT getGradCompLipzConstant(const FT* a, const FT* b, int i) {
        int n = Objective<FT>::getDim();
        FT gl = 0, gu = 0;
        for (int j = 0; j < n; j++) {
            FT l, u;
            PolyObjective <FT>::mHess[i][j].getBounds(a, b, &l, &u);
            BNBInterval<FT>::sqr(l, u, &l, &u);
            gl += l;
            gu += u;
        }
        return sqrt(gu);
    }

private:

    void fillHessBounds(const FT* a, const FT* b) {
        FT varsm[MAX_POLYNOM_VARS * DEG_INTERVAL_MATRIX_ROW_LEN];
        PolyObjective <FT>::mPoly->fillIntervalMatrix(a, b, varsm);
        int n = Objective<FT>::getDim();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                FT l, u;
                PolyObjective <FT>::mHess[i][j].getBoundsCache(varsm, &l, &u);
                mHessBounds[i][2 * j] = l;
                mHessBounds[i][2 * j + 1] = u;
                mHessBounds[j][2 * i] = l;
                mHessBounds[j][2 * i + 1] = u;
            }
        }
    }

    FT mHessBounds[MAX_POLYNOM_VARS][2 * MAX_POLYNOM_VARS];
};

#endif

