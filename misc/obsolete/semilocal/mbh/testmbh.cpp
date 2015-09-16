/* 
 * File:   testmbh.cpp
 * Author: posypkin
 *
 * Test for monotonic basin-hopping
 * Created on September 2, 2011, 9:42 PM
 */

#include <problems/optlib/mbfgs.hpp>
#include <util/common/randomstep.hpp>
#include "basinhop.hpp"
#include "rastr.hpp"
#include "apf.hpp"



#define n 32
#define A 10
#define ampl 1
#define nhop 10
#define eps 0.0001
#define initT 0.00001

int main() {

	Rastr<double> rf(n, A);
	
	Basinhop<double> bh;
	
	UniformCubic<double> rs;
	bh.setRandomStep(&rs);
	bh.setInitStepLength(ampl);
	
	BoltzmannAcceptProbabilityFunction<double> apf;
	apf.setParam(initT);
	bh.setAcceptProbabilityFunction(&apf);
	
	ConstSchedule<double> Tsched;
	bh.setTemperatureSchedule(&Tsched);
	ConstSchedule<double> Rsched;
	bh.setRangeSchedule(&Rsched);
	
	bh.setNumberOfStarts(nhop);
	MBFGS ls;
	ls.setObjective(&rf);
	ls.setEps(eps);
	bh.setLocalOptimizer(&ls);
	double x[n];
	double v;
	for(int i = 0; i < n; i ++) {
		x[i] = 10;
	}
	bh.search(x, &v);
	printf("v = %lf\n", v);
}

