#include <util/common/vec.hpp>
#include <iostream>
#include "elliptic.hpp"
#include "weierstrass.hpp"
#include "griewank.hpp"
#include "ackley.hpp"
#include "rastr.hpp"

#define n 10
int main(){
	Elliptic<double> el(n);
	Weierstrass<double> we(n);
	Griewank<double> gw(n);
	Ackley<double> ac(n);
	Rastr<double> ra(n,2);
	
	double x[n];
	VecUtils::vecSet(n,0.0,x);
	std::cout << el.func(x)<< " "  << we.func(x) << " "  << gw.func(x) << " "  << ac.func(x) << " "  << ra.func(x) << std::endl;
	VecUtils::vecSet(n,1.0,x);
	std::cout << el.func(x)<< " "  << we.func(x) << " "  << gw.func(x) << " "  << ac.func(x) << " "  << ra.func(x) << std::endl;
	return 0;
}
