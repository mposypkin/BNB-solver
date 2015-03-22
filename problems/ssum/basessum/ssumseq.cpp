#include <kernel/dmmemory/bnbseqsolver.hpp>
#include <kernel/dmmemory/schedtracer.hpp>
#include <kernel/dmmemory/seqsched.hpp>
#include <problems/ssum/comssum/ssumutil.hpp>
#include "basessum.hpp"

typedef int T;

SSum<T>* setup(char* fname)
{
    FILE* f = fopen(fname, "r");
    BNB_ASSERT(f);
    char c;
    std::string s;
    while(true) {
        c = fgetc(f);
        if(c == EOF)
            break;
        s += c;
    }
    fclose(f);
    SSum<T>* pr = SSumUtil::fromString<T>(s);
    return pr;
}


void printSolution(SSumSolution<T>& sol) {
    printf("[");
    for(int i = 0; i < sol.mValues.size(); i ++) {
        printf("%d", sol.mValues[i]);
    }
    printf("]=> %d\n", sol.mValue);
}

main(int argc, char* argv[]) {
    BNB_ASSERT(argc == 2);
    SSum<T>* pr = setup(argv[1]);
    BaseSSum<T> res(pr);
    res.generateRoot();
    SeqSched ss;
    ss.setControlInterval(1);
    ss.setSearchStrategyParams(SearchStrategies::WFS, 100, 20000);
    ss.enableTuning(false);
    BNBSeqSolver solver(&ss, &res);
    solver.enableStats(true);


    SchedTracer st(1);
    solver.setTracer(&st);
    solver.solve();

    SSumSolution<T> sol(res.getRecord());
    printSolution(sol);
    
}

