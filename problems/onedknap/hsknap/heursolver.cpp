/* 
 * File:   heursolver.cpp
 * Author: posypkin
 *
 * Created on October 7, 2012, 2:04 PM
 */

#include <problems/onedknap/common/knaputils.hpp>
#include "hsgreedy.hpp"

int main(int argc, char** argv) {
    KnapData<int> kd;
    if (argc != 3)
        BNB_ERROR_REPORT("usage: heursolver.exe <data_file> <heuristic_method>\n");
    KnapUtils<int>::readJSONfromFile(argv[1], kd);
    KnapUtils<int>::sort(kd);
    int hm = atoi(argv[2]);
    HSGreedy<int> hsg(&kd);
    HSSub<int> sub;
    sub = HSUtils<int>::initialSubproblem(kd);
    HSSol<int> sol = HSUtils<int>::emptySolution(kd);
    long long ns = 10;
    hsg.search(sub, sol, ns);
    HSSol<int> rsol = HSUtils<int>::reorderSolution(kd, sol);
    printf("optimal solution: %s\n", HSUtils<int>::HSSol2string(rsol).c_str());

}

