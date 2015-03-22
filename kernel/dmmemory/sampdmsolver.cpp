/* 
 * File:   sampdmsolver.cpp
 * Author: posypkin
 *
 * Sample distributed memory solver
 * 
 * Created on September 16, 2011, 1:18 PM
 */

#include <mpi.h>
#include "util/mplib/mpicommunicator.hpp"
#include "simpsched.hpp"
#include "sampresolver.hpp"
#include "bnbdmsolver.hpp"
#include "schedtracer.hpp"

/**
 * 
 */
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    MpiCommunicator com;
    SimpSched sched(10);
    SampleResolver sr(0, 256, 8);
    SchedTracer st(1);
    BNBDmSolver sol(&com, &sched, &sr);
    sol.setTracer(&st);
    int rank, size;
    rank = com.rank();
    size = com.size();
    sol.solve(rank, size);
    MPI_Finalize();
}

