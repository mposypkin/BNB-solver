#include <kernel/dmmemory/bnbseqsolver.hpp>
#include <kernel/dmmemory/seqsched.hpp>
#include <problems/ssum/comssum/ssumutil.hpp>
#include "basessum.hpp"


typedef int T;

void check1(SSumSolution<T>& sol) {
    BNB_ASSERT(sol.mValue == 6);
}

void check2(SSumSolution<T>& sol) {
    BNB_ASSERT(sol.mValue == 14);
    BNB_ASSERT(sol.mValues[0] == 1);
    BNB_ASSERT(sol.mValues[1] == 1);
    BNB_ASSERT(sol.mValues[2] == 0);
    BNB_ASSERT(sol.mValues[3] == 1);
}

void printSolution(SSumSolution<T>& sol) {
    int N = sol.mValues.size();
    printf("[");
    for (int i = 0; i < N; i++) {
        printf("%d", sol.mValues[i]);
    }
    printf("]=> %d\n", sol.mValue);
}

main() {
    SeqSched ss1;
    ss1.setControlInterval(2);
    ss1.setSearchStrategyParams(SearchStrategies::DFS, 1, 4);
    std::string s = "4 7 4 4 4 2";
    SSum<T>* pr = SSumUtil::fromString<int>(s);
    BaseSSum<T> res1(pr);
    res1.generateRoot();
    BNBSeqSolver solver1(&ss1, &res1);
    solver1.solve();
    SSumSolution<T> sol1(res1.getRecord());
    //printSolution(sol1);
    check1(sol1);

    SeqSched ss2;
    ss2.setControlInterval(2);
    ss2.setSearchStrategyParams(SearchStrategies::DFS, 1, 4);
    delete pr;
    s = "4 15 4 8 16 2";
    pr = SSumUtil::fromString<int>(s);
    BaseSSum<T> res2(pr);
    res2.generateRoot();
    BNBSeqSolver solver2(&ss2, &res2);
    solver2.solve();
    SSumSolution<T> sol2(res2.getRecord());
    //printSolution(sol2);
    check2(sol2);
    delete pr;
}

