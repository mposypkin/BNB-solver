/* 
 * File:   sampdmsolver.cpp
 * Author: posypkin
 *
 * Simple distributed memory solver for
 * coverage method for muticriteria optimization
 * 
 * Created on September 16, 2011, 1:18 PM
 */

#include <mpi.h>
#include <util/mplib/mpicommunicator.hpp>
#include <kernel/dmmemory/simpsched.hpp>
#include <kernel/dmmemory/bnbdmsolver.hpp>
#include <kernel/dmmemory/schedtracer.hpp>
#include <problems/pareto/multicov/multiresolver.hpp>
#include <problems/pareto/multicov/lipbounder.hpp>
#include <problems/pareto/multiopt/multidata.hpp>
#include <problems/pareto/multicov/test1.hpp>
#include <util/boxbnb/boxsub.hpp>
#include <util/boxbnb/boxsubpool.hpp>
#include <problems/pareto/multiopt/mpsimpbag.hpp>
#include <problems/pareto/multiopt/mboxlocsearch.hpp>
#include <problems/pareto/multiopt/mgradesc.hpp>
#include <util/ineq/foumotz.hpp>
#include "testp.hpp"

#define EPS .01
#define DELT .01

/**
 * 
 */
int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    MpiCommunicator com;
    //MultiData<double> md = Test1<double>::fillInData();
    MultiData<double> md = TestP<double>::fillInData(5);

    LipBounder<double> lb(&md);

    MultiCov<double> ml(md);
    ml.addBounder(&lb);
    //ml.getOptions() = multicov<double>::Options::USE_LOCAL_SEARCH;
    FourierMotzkin<double> fm;
    MultiGradDescent<double> mgd(md, &fm);
    ml.addHeuristic(&mgd);
    ml.setPrecision(EPS, DELT);
    MultiState< MPSimpBag < double >, BoxSubPool<double> > ms;
    MultiResolver mr(&ms, &ml);

    int rank, size;
    rank = com.rank();
    size = com.size();
    SchedTracer st(1, "/tmp");
    SimpSched sched(size * 8);

    //sched.setSearchStrategies(SearchStrategies::DFS, SearchStrategies::DFS);
    //sched.setSearchStrategies(SearchStrategies::WFS, SearchStrategies::WFS);

    if (rank == 0) {
        BoxSub<double> sub(md.mBox.mDim, 1);
        sub.mBox = md.mBox;
        ms.mSubBag.put(sub);
    }

    BNBDmSolver sol(&com, &sched, &mr);
    //sol.setTracer(&st);
    sol.setStatsFileName("stats.txt");
    sol.solve(rank, size);
    if (rank == 0) {
        printf("%s", ms.mPointBag.toString().c_str());
    }


    /*
    long long int nit = std::numeric_limits<long long>::max();
    ml.solve(ms, nit);
     */
    MPI_Finalize();
}

