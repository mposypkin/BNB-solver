
#include <mpi.h>
#include <util/mplib/mpicommunicator.hpp>
#include <kernel/dmmemory/simpsched.hpp>
#include <kernel/dmmemory/bnbdmsolver.hpp>
#include <kernel/dmmemory/schedtracer.hpp>
#include <problems/ssum/comssum/ssumutil.hpp>
#include <problems/ssum/basessum/basessum.hpp>
#include <problems/ssum/comssum/ssumser.hpp>

typedef int T;

SSum<T>* setup(char* fname) {
    FILE* f = fopen(fname, "r");
    BNB_ASSERT(f);
    char c;
    std::string s;
    while (true) {
        c = fgetc(f);
        if (c == EOF)
            break;
        s += c;
    }
    fclose(f);
    SSum<T>* pr = SSumUtil::fromString<T > (s);
    return pr;
}

void printSolution(SSumSolution<T>& sol) {
    printf("[");
    for (int i = 0; i < sol.mValues.size(); i++) {
        printf("%d", sol.mValues[i]);
    }
    printf("]=> %d\n", sol.mValue);
}

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    MpiCommunicator com;
    SSum<T> pr(1);
    BinarySerializer ser;
    if (com.rank() == 0) {
        pr = *setup(argv[1]);
        ser << pr;
        for (int p = 1; p < com.size(); p++)
            com.send(ser, p);
    }
    if (com.rank() != 0) {
        com.recv(ser, 0);
        ser >> pr;
    }
    BaseSSum<T> res(&pr);
    if (com.rank() == 0) {
        res.generateRoot();
    }
    SimpSched sched(com.size() * 8);
    SchedTracer st(1, "/tmp");
    BNBDmSolver sol(&com, &sched, &res);
    sol.setStatsFileName("stats.log");
    sol.setTracer(&st);
    sol.solve(com.rank(), com.size());
    SSumSolution<T> solution = res.getRecord();
    if (com.rank() == 0)
        printSolution(solution);

    MPI_Finalize();
}