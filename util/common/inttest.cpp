#include "interval.hpp"

main()
{
  double xmin = 1., xmax = 2., ymin = -1., ymax  = 3.;
  double z1min, z1max, z2min, z2max, z3min, z3max;
  
  BNBInterval<double>::sum(xmin, xmax, ymin, ymax, &z1min, &z1max);
  BNBInterval<double>::diff(xmin, xmax, ymin, ymax, &z2min, &z2max);
  BNBInterval<double>::mult(z1min, z1max, z2min, z2max, &z3min, &z3max);
  printf("result:  [%lf, %lf]\n", z3min, z3max);
  
  xmin = -100;
  xmax = 1;
  BNBInterval<double>::sqr(xmin, xmax, &ymin, &ymax);
  printf("result: [%lf, %lf] (should be [0, 10000])\n", ymin, ymax);
  
  xmin = 1;
  xmax = 100;
  BNBInterval<double>::sin(xmin, xmax, &ymin, &ymax);
  printf("result: [%lf, %lf] (should be [-1, 1])\n", ymin, ymax);
  xmin = (10. * M_PI);
  xmax = (10. * M_PI) + M_PI_2;
  BNBInterval<double>::sin(xmin, xmax, &ymin, &ymax);
  printf("result: [%lf, %lf] (should be [0, 1])\n", ymin, ymax);
  xmin = (- 10. * M_PI) + (M_PI / 6.);
  xmax = (- 10. * M_PI) + M_PI_2;
  BNBInterval<double>::sin(xmin, xmax, &ymin, &ymax);
  printf("result: [%lf, %lf] (should be [0.5, 1])\n", ymin, ymax);
  xmin = (- 10. * M_PI);
  xmax = (- 10. * M_PI) + (M_PI / 3.);
  BNBInterval<double>::cos(xmin, xmax, &ymin, &ymax);
  printf("result: [%lf, %lf] (should be [0.5, 1])\n", ymin, ymax);
  
  xmin = 1;
  xmax = 2;
  double coeff[3] = {1., -1., 1.};
  BNBInterval<double>::polynom(2., coeff, xmin, xmax, &ymin, &ymax);
  printf("result: [%lf, %lf] (should be [1, 3])\n", ymin, ymax);
  
}
