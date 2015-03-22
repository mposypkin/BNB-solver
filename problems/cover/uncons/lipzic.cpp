#include <problems/cover/common/go.hpp>
#include <problems/cover/common/decomposer.hpp>
#include <problems/cover/common/segvecdecomp.hpp>
#include <problems/cover/common/lipzfdiscarder.hpp>
#include <problems/cover/common/lipzgdiscarder.hpp>
#include <problems/optlib/mbfgs.hpp>
#include <kernel/bnblib/seq.hpp>
#include "rosenbrokn.hpp"
#include "booth.hpp"
#include "sqsum.hpp"
#include "barkalov.hpp"
#include "rastrlip.hpp"
#include "grien.hpp"

main(int argc, char* argv[]) {
    typedef GOFactory < double > Fact;
    typedef WTraverse < Fact > Traverse;
    typedef BNBSeqSolver < Traverse > Solver;

    int n;
    double eps = .0001;
    GOInitialData<double> id;
#if 0
    id.mType = GOInitialData<double>::GENERCONSTRAINED;
#endif

#if 0
    n = 2;
    Sqsum obj(n);
    id.mObj = &obj;
    id.mA = (double*) malloc(n * sizeof (double));
    id.mB = (double*) malloc(n * sizeof (double));
    id.mA[0] = -10.;
    id.mA[1] = -10.;
    id.mB[0] = 50.;
    id.mB[1] = 10.;
#endif

#if 0
    Booth obj;
    id.mObj = &obj;
    n = 2;
    id.mA = (double*) malloc(n * sizeof (double));
    id.mB = (double*) malloc(n * sizeof (double));
    for (int i = 0; i < n; i++) {
        id.mA[i] = -10.;
        id.mB[i] = 10.;
    }
#endif

#if 0
    n = 2;
    Rosenbrokn obj(n);
    id.mObj = &obj;
    id.mA = (double*) malloc(n * sizeof (double));
    id.mB = (double*) malloc(n * sizeof (double));
    for (int i = 0; i < n; i++) {
        id.mA[i] = -2.048;
        id.mB[i] = 2.048;
    }
#endif

#if 0
    n = 4;
    Barkalov obj(n, 18);
    id.mObj = &obj;
    id.mA = (double*) malloc(n * sizeof (double));
    id.mB = (double*) malloc(n * sizeof (double));
    for (int i = 0; i < n; i++) {
        id.mA[i] = -1.5;
        id.mB[i] = 1.5;
    }
#endif

#if 0
    n = 6;
    RastrLip obj(n, 1);
    id.mObj = &obj;
    id.mA = (double*) malloc(n * sizeof (double));
    id.mB = (double*) malloc(n * sizeof (double));
    for (int i = 0; i < n; i++) {
        id.mA[i] = -5.12;
        id.mB[i] = 5.12;
    }
#endif

#if 1
    n = 4;
    Grien obj(n);
    id.mObj = &obj;
    id.mA = (double*) malloc(n * sizeof (double));
    id.mB = (double*) malloc(n * sizeof (double));
    for (int i = 0; i < n; i++) {
        id.mA[i] = -700;
        id.mB[i] = 600;
    }
#endif

    LipzFuncDiscarder<double> dscf;
    LipzGradDiscarder<double> dscg;
    Fact fact;

#if 0
    Decomposer<Fact> decomp;
    decomp.setBallExcludeLevel(1);
#else
    SegVecDecomp<Fact> decomp;
#endif
    dscf.setEps(eps);
    dscf.setObjective(&obj);


    dscg.setEps(eps);
    dscg.setObjective(&obj);
    dscg.setInitialData(&id);
#if 1
    /*
    dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_GRAD;
    dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_GRAD_COMP;
    dscg.getOptions() |= LipzGradDiscarder<double>::Options::CHECK_CONV;
     */
    dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_GRAD_BALL_CUT;
    dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_PRIMARY_BALL_CUT;
    dscg.getOptions() |= LipzGradDiscarder<double>::Options::DO_UNCONS_BALL_CUT;

#endif

#if 1
    MBFGS mbfgs;
    mbfgs.setObjective(&obj);
    mbfgs.setEps(0.001);
    fact.setLocalOptimizer(&mbfgs);
    fact.mOptions |= Fact::Options::DO_LOCAL_SEARCH;

#endif

    fact.setInitialData(&id);
    //fact.addDiscarder(&dscf);
    fact.addDiscarder(&dscg);
    fact.addDiscarder(&decomp);
    //dscg.getDebugOptions() |= LipzGradDiscarder<double>::DebugOptions::PRINT_BOUNDS;
    dscg.getDebugOptions() |= LipzGradDiscarder<double>::DebugOptions::PRINT_CONV;
    //fact.getDebugOptions() |= Fact::DebugOptions::DO_DEBUG;
    Solver solver(&fact);
    solver.setMST(100);
    solver.setDebugOptions(Solver::Options::PRINT_RESULTING_STAT);
    //solver.setDebugOptions(solver.getDebugOptions() | Solver::Options::PRINT_STATE);
    //solver.setRecord(n - 1);
    solver.solve();
    printf("Minimum = %s\n", solver.getSolutionContainer()->top().toString().c_str());
    printf("Function calls = %d\n", mbfgs.getSteps());
}
