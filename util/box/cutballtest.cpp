#include <stdio.h>
#include "boxutils.hpp"

#define n 2

main()
{
  double c[n] = {4, 4};
  double r = 5;
  Box<double> box(2);
  box.mA[0] = 0;
  box.mA[1] = 0;
  box.mB[0] = 10;
  box.mB[1] = 5;
  for(int i = 0; i < n; i ++) {
    Segment<double> seg;
    bool rv = BoxUtils::cutL2(i, box, c, r*r, seg);
    if(rv)
      printf("cut across dim %d: (%lf, %lf)\n", i, seg.mA, seg.mB);
    else
      printf("no cut across dim %d\n", i);
  }
  printf("The longest edge has the number %d\n", BoxUtils::getLongestEdge(box));
}
