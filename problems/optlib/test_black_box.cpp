#include <iostream>

#include "util/common/randomstep.hpp"
#include "problems/semilocal/mbh/rastr.hpp"
#include "powell.hpp"
#include "zangwill.hpp"
#include "solis-wets.hpp"

#define n 32
#define A 10


int main(int argc, char** argv) {
	Rastr<double> rf(n, A);
	
	double x[n];
	
	double v;
	
	std::cout << "Testing Solis-Wets1: " << std::endl;
	std::cout << "Creating... " << std::endl;
	SolisWets1<double> sw1;	
	std::cout << "Setting objective... " << std::endl;
	sw1.setObjective(&rf);
	for(int i = 0; i < n; i ++) {
		x[i] = 8;
	};
	std::cout << "Searching... " << std::flush;
	if (sw1.search(x,&v)){
		std::cout << "successful" << std::endl;
		std::cout << "Min found: " << v << " at point: ";
		for(int i = 0; i < n; i ++) {
			std::cout << x[i] <<" ";
		};
		std::cout << std::endl;
	} else {
		std::cout << "failed" << std::endl;
	};
	
	std::cout << "Testing Solis-Wets2: ";
	SolisWets2<double> sw2;
	sw2.setObjective(&rf);
	for(int i = 0; i < n; i ++) {
		x[i] = 8;
	};
	if (sw2.search(x,&v)){
		std::cout << "successful" << std::endl;
		std::cout << "Min found: " << v << " at point: ";
		for(int i = 0; i < n; i ++) {
			std::cout << x[i] <<" ";
		};
		std::cout << std::endl;
	} else {
		std::cout << "failed" << std::endl;
	};
	
	
	std::cout << "Testing Powell: ";
	Powell<double> p;
	p.setObjective(&rf);	
	for(int i = 0; i < n; i ++) {
		x[i] = 0;
	};
	if (p.search(x,&v)){
		std::cout << "successful" << std::endl;
		std::cout << "Min found: " << v << " at point: ";
		for(int i = 0; i < n-1; i ++) {
			std::cout << x[i] <<" ";
		};
		std::cout << std::endl;
	} else {
		std::cout << "failed" << std::endl;
	};
	
	std::cout << "Testing Zangwill: ";
	Zangwill<double> z;
	z.setObjective(&rf);
	
	for(int i = 0; i < n; i ++) {
		x[i] = 8;
	};
	if (z.search(x,&v)){
		std::cout << "successful" << std::endl;
		std::cout << "Min found: " << v << " at point: ";
		for(int i = 0; i < n; i ++) {
			std::cout << x[i] <<" ";
		};
		std::cout << std::endl;
	} else {
		std::cout << "failed" << std::endl;
	};
	

	
};