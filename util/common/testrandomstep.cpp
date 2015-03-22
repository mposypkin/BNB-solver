#include <iostream>
#include <stdio.h>
#include <time.h>

#include "randomstep.hpp"
#include "vec.hpp"



#define n 10

int main(){
	
	srand(time(NULL));
	
	double g[n];
	VecUtils::vecSet<double> (n,1.0,&g[0]);
	HyperPlane<double> p(n,&g[0]);
	double A[n*n];
	p.getMatrix(&A[0]);
	//~ for(int i = 0; i<n*n; ++i){
		//~ std::cout << A[i] << " ";
	//~ }
	//~ std::cout << std::endl;
	
	double x[n-1], y[n];
	UniformSphere<double> us;
	
	double base[n-1];
	VecUtils::vecSet<double> (n-1,0.0,&base[0]);
	double r = 1.0;
	double m = 0.0;
	for(int i = 0; i<100; ++i){
		std::cout << "=" << std::flush;
		for(int j = 0; j<30; ++j){
			us.next(n-1,&base[0],&r,&x[0]);
			p.convertCoords(&x[0],&y[0]);
			if (VecUtils::vecScalarMult<double>(n, &g[0], &y[0]) > m)
				m = VecUtils::vecScalarMult<double>(n, &g[0], &y[0]);
			//~ std::cout << VecUtils::vecNormTwo<double>(n, &y[0]) << " " << VecUtils::vecScalarMult<double>(n, &g[0], &y[0]) << std::endl;
		}
	}
	std::cout<<std::endl;
	std::cout<<m<<std::endl;
	
	
	HyperPlaneUniformSphere<double> hpus(n,&g[0]);
	
	m = 0.0;
	for(int i = 0; i<10; ++i){
		hpus.next(n,&base[0],&r,&x[0]);
		for(int j = 0; j<n; ++j){
			std::cout << x[j] << " " << std::flush;
		};
		std::cout<<std::endl;
		std::cout << VecUtils::vecNormTwo<double>(n, x) << " " << VecUtils::vecScalarMult<double>(n, &g[0], x) << std::endl;		
		//~ std::cout << "=" << std::flush;
		//~ for(int j = 0; j<30000; ++j){
			//~ hpus.next(n,&base[0],&r,&x[0]);
			//~ if (VecUtils::vecScalarMult<double>(n, &g[0], &y[0]) > m)
				//~ m = VecUtils::vecScalarMult<double>(n, &g[0], &y[0]);
			//~ std::cout << VecUtils::vecNormTwo<double>(n, &y[0]) << " " << VecUtils::vecScalarMult<double>(n, &g[0], &y[0]) << std::endl;
		//~ }
	}
	std::cout<<std::endl;
	//~ std::cout<<m<<std::endl;
	
	return 0;	
}

