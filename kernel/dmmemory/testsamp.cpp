#include <stdio.h>
#include<util/common/bnberrcheck.hpp>
#include "sampresolver.hpp"


main() {
    SampleResolver sr(0, 256, 8);
    long long n = 7;
    sr.solve(n);
    BNB_ASSERT(n == 7);
    BinarySerializer bs;
    sr.getRecords(bs);
    double rec;
    bs.reset();
    bs >> rec;
    BNB_ASSERT(rec == 128.0);
    bs.reset();
    int nsub = 1000;
    sr.getSubs(nsub, bs);
    bs.reset();
    bs >> n;
    BNB_ASSERT(n == 4);
#if 0
    for(int i = 0; i < n; i ++) {
        double a, b;
        bs >> a;
        bs >> b;
        printf("[%lf, %lf]", a, b);
    }
    printf("\n");
#endif
}

