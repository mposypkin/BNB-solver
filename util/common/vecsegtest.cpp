#include "vecseg.hpp"

main()
{
  VecSeg<4, double> vs;
  for(;;) {
    Segment<double> s;
    printf("Enter a, b => ");
    scanf("%lf", &(s.mA));
    scanf("%lf", &(s.mB));
    vs.push(s);
    vs.print();
  }
}
