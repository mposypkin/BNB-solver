 //~ -*- coding: utf-8 -*-
#ifndef __APF__
#define __APF__

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <fstream>
#include <util/common/gabriel.hpp>
#include <util/common/vec.hpp>
#include <util/common/bnblog.hpp>


/**
* Abstract class for realisation of different functoions of AP
*/

template <typename FT> class AcceptProbabilityFunction{
public:
	/**
	* Constructors
	*/
	AcceptProbabilityFunction(){
		T = 0.0;
	};
	
	AcceptProbabilityFunction(FT newT){
		T = newT;
	};

	/**
	* Calculate probabilty of upward step of given height df
	* @param df difference between values of function in the beginning and in the end of step in question
	* @return probability of such step - number in the interval [0,1]
	*/
	virtual FT prob(FT df) = 0;

	/**
	* get the value of "temperature" parameter
	* @return "temperature"
	*/
	FT getParam(){
		return T;
	};

	/**
	* set the value of "temperature" parameter
	* @param "temperature"
	*/
	void setParam(FT newT){
		T = newT;
	};

protected:
	FT T;
};

template <typename FT> class BoltzmannAcceptProbabilityFunction: public AcceptProbabilityFunction<FT>{
public:
	BoltzmannAcceptProbabilityFunction(){};
	BoltzmannAcceptProbabilityFunction(FT newT){};
	using AcceptProbabilityFunction<FT>::T;
	FT prob(FT df){
		if (T>0.0){
			return exp (-df/T);
		} else {
			return 0;
		};
	};
};

template <typename FT> class ConstAcceptProbabilityFunction: public AcceptProbabilityFunction<FT>{
public:
	ConstAcceptProbabilityFunction(){};
	ConstAcceptProbabilityFunction(FT newT){};
	using AcceptProbabilityFunction<FT>::T;
	FT prob(FT df){
		return T;
	};
};

template <typename FT> class Schedule {
public:	
	virtual FT getParam (int num_of_steps, int num_of_success, int num_of_failures, FT* pos ,FT obj_val, FT param) = 0;
};

template <typename FT> class ConstSchedule: public Schedule<FT> {
public:	
	FT getParam (int num_of_steps, int num_of_success, int num_of_failures, FT* pos ,FT obj_val, FT param) {
		return param;
	}
};

template <typename FT> class RandomSchedule: public Schedule<FT> {
public:	
	RandomSchedule(FT min_, FT max_){
		min = min_;
		max = max_;
		srand(time(NULL));
	}
	
	FT getParam (int num_of_steps, int num_of_success, int num_of_failures, FT* pos ,FT obj_val, FT param) {
		r = ((FT)rand())/ ((FT)RAND_MAX);
		return r * max + (1-r) * min;
	}
		
private:
	FT min, max,r;
};




template <typename FT> class StatisticSchedule: public Schedule<FT> {
public:	
	

	StatisticSchedule(FT min_, FT max_, int num_, int init){
		//~ std::cerr<<"Creating statistics schedule " << std::endl;
		if (min_ > 0){
			min = min_;
		} else {
			min = 0.0;
		};
		max = max_;
		num = num_;
		statsum = num*init;
		step = (max-min)/num;
		srand(time(NULL));
		stat = (int*)malloc(num*sizeof(int));
		for(int j = 0; j < num; ++j){
			stat[j] = init;
		};
		succ = 0;
		//~ std::cerr<<"Statistics schedule created" << std::endl;
	};

	StatisticSchedule(){
		//~ std::cerr<<"Creating statistics schedule " << std::endl;
		min = 0.0;
		max = 1.0;
		num = 10;
		int init = 1;
		statsum = num*init;
		step = (max-min)/num;
		srand(time(NULL));
		stat = (int*)malloc(num*sizeof(int));
		for(int j = 0; j < num; ++j){
			stat[j] = init;
		};
		succ = 0;
		//~ std::cerr<<"Statistics schedule created" << std::endl;
	};
	
	void ResetParams(FT min_, FT max_, int num_, int init){
		if (min_ > 0){
			min = min_;
		} else {
			min = 0.0;
		};
		max = max_;
		num = num_;
		statsum = num*init;
		step = (max-min)/num;
		srand(time(NULL));
		stat = (int*)realloc(stat,num*sizeof(int));
		for(int j = 0; j < num; ++j){
			stat[j] = init;
		};
		succ = 0;
	}
	
	FT getParam (int num_of_steps, int num_of_success, int num_of_failures, FT* pos ,FT obj_val, FT param) {
		// Update stats
		if ((num_of_steps > 1) and (num_of_success>succ)){
				succ = num_of_success;
				stat[pi]+=1;
		};
		
		//Choose step
		c = rand() % statsum;
		int i = 0;
		int sum = 0;
		while (c > sum){
			sum+=stat[i];
			i++;
		}
		//Save chosen step to use in next iteration
		pi = i;
		//Choose random point in step
		r = ((FT)rand())/ ((FT)RAND_MAX);
		return min+step*i+step*r;
	};
		
	~StatisticSchedule(){
		free(stat);
	};
private:
	FT min, max,r,step;
	int num, statsum,c,pi, succ;
	int* stat;
};

template <typename FT> class GabrielSchedule: public Schedule<FT> {
public:
	GabrielSchedule(int dim,int num_of_stored_points, FT init_step_length){
		g = new Gabriel<FT>(dim);
		g->Reserve(num_of_stored_points);
		nosp = num_of_stored_points;
		g->SetSamePointsCheck(1e-6);
		isl = init_step_length;
		
	};
	
	FT getParam (int num_of_steps, int num_of_success, int num_of_failures, FT* pos ,FT obj_val, FT param) {
		if (g->n_points < nosp){
			g->AddPoint(pos);
		};
		if (g->n_edges>0){
			FT av_rad = 0.0;
			for(int j = 0; j < g->n_edges; ++j){
				av_rad+=g->radii[j];
			};
			return av_rad/g->n_edges;
		} else {
			return isl;
		}
	}
	
	~GabrielSchedule(){
		delete g;
	}
	
private:
	Gabriel<FT>* g;
	FT isl;
	int nosp;
};

template <typename FT> class LinearSchedule: public Schedule<FT> {
public:
	LinearSchedule(FT iP, int tnos){
		initParam = iP;
		target_num_of_steps = tnos;
	};
	
	FT getInitParam(){
		return initParam;
	};
	
	int getTargetNumberOfSteps(){
		return target_num_of_steps;
	};
	
	void setInitParam(FT iP){
		initParam = iP;
	};

	void setTargetNumberOfSteps(int tnos){
		target_num_of_steps = tnos;
	};

	FT getParam (int num_of_steps, int num_of_success, int num_of_failures, FT* pos ,FT obj_val, FT param) {
		if (num_of_steps < target_num_of_steps){
			return initParam* (1.0-(FT)num_of_steps/(FT)target_num_of_steps);
		} else {
			return 0.0;
		}
	};
private:
	FT initParam;
	int target_num_of_steps;
};

template <typename FT> class InvLogSchedule: public Schedule<FT> {
public:
	InvLogSchedule(FT iP, FT tps){
		initParam = iP;
		time_per_step = tps;
	};
	FT getInitParam(){
		return initParam;
	};
	void setInitParam(FT iP){
		initParam = iP;
	};
	
	void setTimePerStep(FT tps){
		time_per_step = tps;
	};
	FT getTimePerStep(){
		return time_per_step;
	};
	FT getParam (int num_of_steps, int num_of_success, int num_of_failures, FT* pos ,FT obj_val, FT param) {
		return initParam / log (1.0+time_per_step*num_of_steps);
	};
private:
	FT initParam;
	FT time_per_step;	
	
};

template <typename FT> class SolisWetsSchedule: public Schedule<FT> {
public:
	
	SolisWetsSchedule(){
		target_nos = 1;
		target_nof = 40;
		prev_nos = 0;
		prev_nof = 0;
		isSuccess = true;
		coef_expand = 1.1;
		coef_collaps = 0.5;
		nocf = 0;
		nocs = 0;
	}
	
	void setTargetNOS(int nos){
		target_nos = nos;
	}

	void setTargetNOF(int nof){
		target_nof = nof;
	}

	FT getParam (int num_of_steps, int num_of_success, int num_of_failures, FT* pos ,FT obj_val, FT param) {
		
		mLog <<"SWS: " << "Initiated: nos = " << num_of_success << ", nof = " << num_of_failures << ", nof = " << param<< "\n";
		mLog.write();
		if (num_of_success > prev_nos){
			nocs+=1;
			nocf=0;
			prev_nos = num_of_success;
			mLog <<"SWS: " << "Success: nocs = " << nocs << "\n";
			mLog.write();
			
			mLog <<"SWS: " << "Target nos of " << target_nos;
			if (nocs >= target_nos){
				mLog << " reached. Returning " << param*coef_expand << "\n";
				mLog.write();
				nocs = 0;
				return param*coef_expand;
			} else {
				mLog << " not reached. Returning " << param << "\n";
				mLog.write();
				return param;
			};
		} ;

		if (num_of_failures > prev_nof){
			nocf+=1;
			nocs=0;
			prev_nof = num_of_failures;
			mLog <<"SWS: " << "Failure: nocf = " << nocf << "\n";
			mLog.write();
			mLog <<"SWS: " << "Target nof of " << target_nof;
			if (nocf >= target_nof){
				mLog << " reached. Returning " << param*coef_collaps << "\n";
				mLog.write();
				nocf = 0;
				return param*coef_collaps;
			} else {
				mLog << " not reached. Returning " << param << "\n";
				mLog.write();
				return param;
			};
		};
		
		return param;
	};

private:
	int target_nos, target_nof, prev_nos, prev_nof,nocs,nocf;
	FT coef_expand, coef_collaps;
	bool isSuccess;
	BNBLog <BNB_NO_LOGGING> mLog;
	
};

#endif

