//~ -*- coding: utf-8 -*-
#ifndef __SIM_ANNEAL__
#define __SIM_ANNEAL__

#include <stdlib.h>
#include <math.h>
#include <problems/optlib/locoptim.hpp>
#include <problems/optlib/null.hpp>
#include <util/common/bnblog.hpp>
#include <util/common/randomstep.hpp>
#include "apf.hpp"
#include "basinhop.hpp"

template <typename FT> class SimAnneal : public  Basinhop <FT>{
	/**
	* The constructor
	*/
	SimAnneal(FT initial_temperature, FT cooling_speed, FT isl){
		NullOpt<FT> opt;
		setLocalOptimizer(&opt);
		BoltzmannAcceptProbabilityFunction<FT> apf(initial_temperature);
		setAcceptProbabilityFunction(&apf);
		InvLogSchedule<FT> Sched(initial_temperature,cooling_speed);
		setTemperatureSchedule(& Sched);
		UniformCubic<FT> rs;
		setRandomStep(&rs);
		setInitStepLength(isl);
	}
};




#endif