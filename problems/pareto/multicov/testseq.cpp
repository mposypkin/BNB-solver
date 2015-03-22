#include <kernel/dmmemory/seqsched.hpp>
#include <kernel/dmmemory/schedtracer.hpp>
#include <kernel/dmmemory/bnbseqsolver.hpp>
#include <problems/pareto/multiopt/mboxlocsearch.hpp>
#include <problems/pareto/multiopt/mgradesc.hpp>
#include <util/ineq/foumotz.hpp>
#include "multiresolver.hpp"
#include "test1.hpp"


#define EPS 0.25
#define DELT 0.01

main() {
    MultiData<double> md = Test1<double>::fillInData();
    LipBounder<double> lb(&md);
    MultiCov<double> ml(md);
    ml.addBounder(&lb);
    ml.setPrecision(EPS, DELT);
    FourierMotzkin<double> fm;
    MultiGradDescent<double> mgd(md, &fm);
    ml.addHeuristic(&mgd);
//    ml.setHeuristic(1);

    MultiState< MPSimpBag < double >, BoxSubPool<double> > ms;
    MultiResolver mr(&ms, &ml);

    SeqSched ss;
    ss.setControlInterval(4);
    ss.setSearchStrategyParams(SearchStrategies::WFS, 16, 128);
    ss.setHeuristic(1);
    BNBSeqSolver solver(&ss, &mr);
    SchedTracer st(1);
    solver.setTracer(&st);

    BoxSub<double> sub(md.mBox.mDim, 1);
    sub.mBox = md.mBox;
    ms.mSubBag.put(sub);

    printf("hehe\n");
    solver.solve();
//    ml.solve(ms, 10000, NULL);
    printf("%s", ms.mPointBag.toString().c_str());

}
