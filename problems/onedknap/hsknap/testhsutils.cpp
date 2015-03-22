#include <problems/onedknap/common/knaputils.hpp>
#include "hsutils.hpp"
#include "u0bounder.hpp"

#define JSON_STR "{ \"N\" : 4,\"C\" : 9,\"W\" : [2, 3, 5, 7],\"P\" : [5, 7, 6, 3]}"

#define S1 "fro: -1, h: 0, p: 0, w: 0, 2222"
#define S2 "0, 0000"
#define S3 "fro: -1, h: 0, p: 5, w: 2, 1222"
#define S4 "5, 1000"

main() {

    std::string json = JSON_STR;
    KnapData<int> kd;
    KnapUtils<int>::readJson(json, kd);
    KnapUtils<int>::sort(kd);
    HSSub<int> sub;
    sub = HSUtils<int>::initialSubproblem(kd);
    std::string s = HSUtils<int>::HSSub2string(sub);
    //printf("%s\n", s.c_str());
    if (s != S1)
        BNB_ERROR_REPORT("Wrong string");
    HSSol<int> sol;
    sol = HSUtils<int>::emptySolution(kd);
    s = HSUtils<int>::HSSol2string(sol);
    //printf("%s\n", s.c_str());
    if (s != S2)
        BNB_ERROR_REPORT("Wrong string");
    sub.mH = 0;
    sub.mX[0] = 1;
    sub.mP += kd.mCoe[0].mP;
    sub.mW += kd.mCoe[0].mW;
    U0Bounder<int> bou;
    bool f = bou.BoundAndReduce(kd, sub, sol);
    //printf("%s\n", f ? "discard" : "expand");
    if (f)
        BNB_ERROR_REPORT("Wrong bool value");
    s = HSUtils<int>::HSSub2string(sub);
    //printf("%s\n", s.c_str());
    if (s != S3)
        BNB_ERROR_REPORT("Wrong string");
    s = HSUtils<int>::HSSol2string(sol);
    //printf("%s\n", s.c_str());
    if(s != S4)
        BNB_ERROR_REPORT("Wrong string");
    return 0;
}

