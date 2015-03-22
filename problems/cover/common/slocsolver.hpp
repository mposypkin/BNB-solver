#ifndef __SLOCSOLVER_HPP__
#define __SLOCSOLVER_HPP__

#include <sstream>
#include <ei/factory/serialsolver.hpp>
#include <ei/external/bnbcommand.hpp>
#include <ei/external/bnbcmdprocessor.hpp>
#include <util/xml/xmlpars.hpp>
#include <util/common/smartptr.hpp>

/**
 * Sequential local solver
 */
class SLocSolver : public SerialSolver {
public:

    struct Solution {

        Solution(int dim) : mX(dim) {
            mDim = dim;
        }
        SmartArrayPtr<double> mX;
        double mFval;
        int mDim;
    };

    SLocSolver() {
        mLocOpt = NULL;
        mBestSolutionIndex = 0;
        mBestSolutionValue = DBL_MAX;
        mDelta = 0.0;
    }

    void setLocalOptimizer(LocalOptimizer<double> *opt) {
        mLocOpt = opt;
        mDim = opt->getObjective()->getDim();
        //mAux = (VT*) realloc(mAux, mDim * sizeof(VT));
    }

    void setDelta(double delta) {
        mDelta = delta;
    }

    void solve() {
        BNB_ASSERT(mLocOpt);
        int action;
        BNBCommandProcessor cmdp;
        while (1) {
            if (mGate->probe()) {
                BNBCommand cmd;
                if (!mGate->get(cmd))
                    continue;
                printf("SLOCSOLVER got command: %s\n", cmd.mName.c_str());
                cmdp.process(cmd, action);
                if (cmd.mName == BNB_PUT_SOLUTIONS) {
                    if (cmd.mDsc != BNB_NIL) {

                        class SP : public XMLListener {
                        public:

                            SP(SLocSolver* solver) {
                                mSolver = solver;
                            }

                            void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end) {
                                if (tag == BNB_SOLUTION) {
                                    std::string str(s, begin, end - begin + 1);
                                    Solution sol(mSolver->getDim());
                                    mSolver->getPointFromString(str, sol);
                                    mSolver->getPoints().push_back(sol);
                                }
                            }
                        private:
                            SLocSolver* mSolver;
                        };

                        SP spl(this);
                        XMLPars pars;
                        pars.addListener(&spl);
                        pars.parse(cmd.mDsc);
                    }
                } else if (cmd.mName == BNB_START_SOLVE) {
                    int sz = mPoints.size();
                    mBestSolutionIndex = -1;
                    for (int i = 0; i < sz; i++) {
                        mBestSolutionValue = BNBMIN(mBestSolutionValue, mPoints[i].mFval);
                    }
                    for (int i = 0; i < sz; i++) {
                        mLocOpt->search(mPoints[i].mX, &(mPoints[i].mFval));
                        if (mPoints[i].mFval < mBestSolutionValue - mDelta) {
                            mBestSolutionIndex = i;
                            mBestSolutionValue = mPoints[i].mFval;
                        }
                    }
                    if (mBestSolutionIndex >= 0) {
                        std::ostringstream os;
                        os << "<solu>";
                        os << "<x>";
                        for (int i = 0; i < mDim; i++) {
                            os << mPoints[mBestSolutionIndex].mX[i];
                            if (i != (mDim - 1))
                                os << " ";
                        }
                        os << "</x>";
                        os << "<f>";
                        os << mBestSolutionValue;
                        os << "</f>";
                        os << "</solu>";
                        BNBCommand lcmd;
                        lcmd.mName = BNB_INCUMBENT_UPDATE;
                        lcmd.mDsc = os.str();
                        mGate->put(lcmd);
                    }
                }
            } else
                break;
        }

    }

    int getDim() const {
        return mDim;
    }

    std::vector< Solution >& getPoints() {
        return mPoints;
    }

    void getPointFromString(std::string& s, Solution& sol) {

        class SP : public XMLListener {
        public:

            SP(Solution* sol) {
                mSol = sol;
            }

            void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end) {
                std::string ss(s, begin, end - begin + 1);
                std::istringstream iss(ss);
                if (tag == "x") {
                    int n;
                    n = mSol->mDim;
                    for (int i = 0; i < n; i++) {
                        if (!iss.good()) {
                            fprintf(stderr, "parsing element %d from %d\n", i, n);
                            BNB_ERROR_REPORT("Parsing X-vector for a point failed\n");
                        }
                        iss >> mSol->mX[i];
                    }
                } else if (tag == "f") {
                    iss >> mSol->mFval;
                    if (iss.bad())
                        BNB_ERROR_REPORT("Parsing func value for a Solution failed\n");
                }
            }
        private:
            Solution* mSol;
        };

        SP spl(&sol);
        XMLPars pars;
        pars.addListener(&spl);
        pars.parse(s);
    }

private:

    LocalOptimizer<double>* mLocOpt;
    int mDim;
    std::vector< Solution > mPoints;
    int mBestSolutionIndex;
    double mBestSolutionValue;
    double mDelta;
};

#endif
