/* 
 * File:   seqsolver.cpp
 * Author: posypkin
 *
 * Created on September 18, 2012, 11:54 AM
 */
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <kernel/dmmemory/seqsched3.hpp>
#include <kernel/dmmemory/bnbseqsolver.hpp>
#include <kernel/dmmemory/simptracer.hpp>
#include <kernel/dmmemory/schedtracer.hpp>
#include <problems/onedknap/common/knaputils.hpp>
#include "hsutils.hpp"
#include "u0bounder.hpp"
#include "hsresolver.hpp"
#include "lpbounder.hpp"
#include "mtbounder.hpp"
#include "smartbounder.hpp"
#include "hsgreedy.hpp"
#include "hsntuple.hpp"

typedef int T;

int main(int argc, char** argv) {
    KnapData<T> kd;
    int rv = 0;
    if (argc < 5)
        BNB_ERROR_REPORT("usage: seqsolver.exe <data_file> <bounding_method> <init_heuristic> <main_heuristic> record(optional)\n");
    KnapUtils<T>::readJSONfromFile(argv[1], kd);
    int bm = atoi(argv[2]);
    int iheur = atoi(argv[3]);
    int mheur = atoi(argv[4]);
    if(argc >= 6) {
        rv = atoi(argv[5]);
    }
    KnapUtils<T>::sort(kd);
//    printf("%s\n", KnapUtils<int>::writeJson(kd).c_str());
    HSResolver<T> hsr(&kd);
    HSGreedy<T> hsg(&kd);
    hsr.addHeur(&hsg);
    HSNTuple<T> hsntup(&kd, 2);
    hsr.addHeur(&hsntup);
    U0Bounder<T> bnd;
    hsr.addBounder(&bnd);
    LPBounder<T> lpbnd;
    hsr.addBounder(&lpbnd);
    MTBounder<T> mtbnd;
    hsr.addBounder(&mtbnd);
    SmartBounder<T> smrtbnd;
    hsr.addBounder(&smrtbnd);
    if(rv > 0)
        hsr.setRecordValue(rv);
    HSSub<T> sub;
    sub = HSUtils<T>::initialSubproblem(kd);
    hsr.fetchSub(sub);

    SeqSched3 ss;
    ss.getParams().mMB = bm;
    ss.getParams().mIH = iheur;
    ss.getParams().mMH = mheur;
    ss.getParams().mMBRS = 1000;
    BNBSeqSolver solver(&ss, &hsr);
    //SchedTracer st(1);
    SimpTracer st;
    solver.setTracer(&st);
    solver.solve();
    HSSol<T> rsol = HSUtils<T>::reorderSolution(kd, hsr.getRecord());
    //printf("optimal solution: %s\n", HSUtils<int>::HSSol2string(rsol).c_str());
    printf("optimal solution: %d\n", rsol.mP);
    printf("%d steps performed\n", st.getInfo().mNSteps);
}

