#include "sampresolver.hpp"
#include "seqsched.hpp"
#include "bnbseqsolver.hpp"
#include "schedtracer.hpp"

main() {
    SampleResolver sr(0, 256, 4);
    SeqSched ss;
    BNBSeqSolver solver(&ss, &sr);
    SchedTracer st(1);
    solver.setTracer(&st);
    solver.solve();
}
