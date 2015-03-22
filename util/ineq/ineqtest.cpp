/* 
 * File:   ineqtest.cpp
 * Author: posypkin
 *
 * Created on April 25, 2011, 12:27 AM
 */

#include "foumotz.hpp"
#include "util/common/vec.hpp"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    double a[2][3] = {
        {2, -1, 0},
        {-1, 2, 0}
    };
    double p[2];
    FourierMotzkin<double> fm;
    fm.setInequalities(2, 2, (double*) a);
    if (fm.findFeasible(p)) {
        printf("feasible:");
        VecUtils::vecPrint(2, p);
        BNB_ASSERT(p[0] == -1.25);
        BNB_ASSERT(p[1] == -1);
    } else
        printf("No feasible vector\n");

    double a1[4][4] = {
        {1, -1, 0, 0},
        {1, 0, -1, 0},
        {-1, 1, 2, 0},
        {0, 0, -1, 1}
    };
    double p1[3];
    fm.setInequalities(3, 4, (double*) a1);
    if (fm.findFeasible(p1)) {
        printf("feasible:");
        VecUtils::vecPrint(3, p1);
        BNB_ERROR_REPORT("No feasible vector exists");
    } else
        printf("No feasible vector\n");

    double a2[2][3] = {
        {1, -1, 0},
        {-1, 1, 0}
    };
    double p2[2];
    fm.setInequalities(2, 2, (double*) a2);
    if (fm.findFeasible(p2)) {
        printf("feasible:");
        VecUtils::vecPrint(2, p2);
        BNB_ERROR_REPORT("No feasible vector exists");
    } else
        printf("No feasible vector\n");

    double a3[3][3] = {
        {1, 1, 0},
        {0, -1, 0},
        {-1, 0, 0}

    };
    double p3[2];
    fm.setInequalities(2, 3, (double*) a3);
    if (fm.findFeasible(p3)) {
        printf("feasible:");
        VecUtils::vecPrint(2, p3);
        BNB_ERROR_REPORT("No feasible vector exists");
    } else
        printf("No feasible vector\n");

    double a4[4][3] = {
        {1, 2, 0},
        {2, 1, 0},
        {1, 0, 0},
        {0, 1, 0}
    };
    double p4[2];
    fm.setInequalities(2, 4, (double*) a4);
    if (fm.findFeasible(p4)) {
        printf("feasible:");
        VecUtils::vecPrint(2, p4);
        BNB_ASSERT(p4[0] == -1);
        BNB_ASSERT(p4[1] == -1);
    } else
        printf("No feasible vector\n");


    return 0;
}

