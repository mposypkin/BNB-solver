#include <problems/onedknap/common/knaputils.hpp>
#include "hsutils.hpp"
#include "u0bounder.hpp"
#include "hsresolver.hpp"

#define JSON_STR "{\"N\":7,\"C\":9,\"W\":[2,3,6,7,5,9,4],\"P\":[6,5,8,9,6,7,3]}"
//#define JSON_STR "{ \"N\" : 4,\"C\" : 9,\"W\" : [2, 3, 5, 7],\"P\" : [5, 7, 6, 3]}"
//#define JSON_STR "{ \"N\" : 4,\"C\" : 5,\"W\" : [2, 2, 2, 2],\"P\" : [2, 2, 2, 2]}"
//#define JSON_STR "{ \"N\" : 3,\"C\" : 4,\"W\" : [2, 2, 2],\"P\" : [2, 2, 2]}"
//#define JSON_STR "{ \"N\" : 3,\"C\" : 7,\"W\" : [2, 2, 2],\"P\" : [2, 2, 2]}"
//#define JSON_STR "{ \"N\" : 6,\"C\" : 14,\"W\" : [2, 3, 5, 7, 7, 7],\"P\" : [5, 7, 6, 3, 2, 2]}"

main() {
    std::string json = JSON_STR;
    KnapData<int> kd;
    KnapUtils<int>::readJson(json, kd);
    KnapUtils<int>::sort(kd);
    HSResolver<int> hsr(&kd);
    U0Bounder<int> bnd;
    hsr.addBounder(&bnd);

    HSSub<int> sub;
    sub = HSUtils<int>::initialSubproblem(kd);
    long long nit;
    hsr.DFSearch(1000, nit, sub);
    printf("%lld iterations, %s\n", nit, HSUtils<int>::HSSol2string(hsr.getRecord()).c_str());
    BNB_ASSERT(hsr.getRecord().mP == 15);

    hsr.reset();
    sub = HSUtils<int>::initialSubproblem(kd);
    hsr.fetchSub(sub);
    hsr.WFSearch(1000, nit);
    printf("%lld iterations, %s\n", nit, HSUtils<int>::HSSol2string(hsr.getRecord()).c_str());
    BNB_ASSERT(hsr.getRecord().mP == 15);

    hsr.reset();
    sub = HSUtils<int>::initialSubproblem(kd);
    hsr.fetchSub(sub);
    hsr.setSearchStrategy(SearchStrategies::DFS);
    nit = 1000;
    hsr.solve(nit);
    printf("%lld iterations, %s\n", nit, HSUtils<int>::HSSol2string(hsr.getRecord()).c_str());
    BNB_ASSERT(hsr.getRecord().mP == 15);

    hsr.reset();
    sub = HSUtils<int>::initialSubproblem(kd);
    hsr.fetchSub(sub);
    hsr.setSearchStrategy(SearchStrategies::WFS);
    nit = 1000;
    hsr.solve(nit);
    printf("%lld iterations, %s\n", nit, HSUtils<int>::HSSol2string(hsr.getRecord()).c_str());
    BNB_ASSERT(hsr.getRecord().mP == 15);
}
