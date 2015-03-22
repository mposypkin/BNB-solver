#include "qubic.hpp"

main()
{
  int nr;
  double roots[3];
  Qubic<double> quib(1., -6., 11., -6.);
  nr = quib.solve(roots);
  printf("%d roots\n", nr);
  for(int i = 0; i < nr; i ++) {
    printf("%lf\n", roots[i]);
  }
  printf("Minimum on [1, 3] = %lf\n", quib.findMinimum(1., 3.));
}
