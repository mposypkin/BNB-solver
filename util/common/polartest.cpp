#include <stdio.h>
#include "polar.hpp"

main()
{
  double phi[1];
  double x[2];
  double g[2];
  double ag[1];
  
  phi[0] = 0;
  Polar::polar2cart(2, 1., phi, x);
  printf("1 = %lf, 0 = %lf\n", x[0], x[1]);
  phi[0] = 0.5 * M_PI;
  Polar::polar2cart(2, 1., phi, x);
  printf("0 = %lf, 1 = %lf\n", x[0], x[1]);
  Polar::polar2cart(2, 1., phi, x);
  phi[0] = 1.;
  Polar::polar2cart(2, 1., phi, x);
  printf("%lf, %lf\n", x[0], x[1]);
  g[0] = 2. * x[0];
  g[1] = 2. * x[1];
  Polar::polarAngleGrad(2, 1., phi, g, ag);
  printf("ag[0] = %lf\n", ag[0]);
}
