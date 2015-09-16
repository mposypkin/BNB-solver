 //~ -*- coding: utf-8 -*-
#ifndef __BASINHOP__
#define __BASINHOP__

#include <stdlib.h>
#include <math.h>
#include <problems/optlib/locoptim.hpp>
#include <util/common/bnblog.hpp>
#include <util/common/randomstep.hpp>
#include "apf.hpp"

#define epsilon 0.05

template <typename FT> class Basinhop : public  LocalOptimizer <FT>{
public:
    
	/**
	* The constructor
	*/
	Basinhop(){
		mAux = NULL;
		mBest = NULL;
		mOpt = NULL;
		//~ mAPF = &BoltzmannAcceptProbabilityFunction<FT>();
		//~ mRS = &UniformSphere();
		//~ mTempSched = &ConstSchedule<FT>();
		mAPF = NULL;
		mRS = NULL;
		mTempSched = NULL;
		mRangeSched = NULL;
		
		l_bound  = NULL;
		u_bound = NULL;
	}

	~Basinhop(){
		if(mAux)
			free(mAux);
		if(mBest)
			free(mBest);
		if(l_bound)
			free(l_bound);
		if(u_bound)
			free(u_bound);
	}


	/**
	* Retrieve objective
	* @return objective
	*/
	Objective<FT>* getObjective()
	{
		return mOpt->getObjective();
	}


	/**
	* Perform search
	* @param x start point and result
	* @param v 
	* @return true if search converged and false otherwise
	*/
	bool search(FT* x, FT* v){

		int n, nos,nof,step_count, noob; // Number_of_success, number_of_failure
		FT vbest,very_best,fAux;
		//~ mLog <<"BH: "<< "Initial jump radius =  " << init_step_length << "\n";
		//~ mLog.write();
		FT ampl = init_step_length;
		FT rand_num;
		FT T = mAPF->getParam();
		n = mObj->getDim();

				
		vbest = mObj->func(x);
		very_best = vbest;
		nos = 0;
		nof = 0;
		step_count = 0;
		noob = 0;
		
		mLog <<"BH: MARK_A: "<< "Starting point: " ;
		for(int j = 0; j < n; j ++) {
			mLog << x[j] << " ";
		};
		mLog << "\n";
		mLog.write();
		
		mLog <<"BH: MARK_A: "<< "Old mBest point: " ;
		for(int j = 0; j < n; j ++) {
			mLog << mBest[j] << " ";
		};
		mLog << "\n";
		mLog.write();

		//~ std::cout << "I'm doing this!" << std::endl;
		for(int j = 0; j < n; j ++) {
			mBest[j] = x[j];
		};
		
		mLog <<"BH: MARK_A: "<< "New mBest point: " ;
		for(int j = 0; j < n; j ++) {
			mLog << mBest[j] << " ";
		};
		mLog << "\n";
		mLog.write();
		char alive[4] = {'\\', '|','/','-'};
		int alive_count = 0;
		for(int i = 0; i < mNumberOfStarts; i ++) {
			std::cerr << alive[alive_count] << " " << vbest << '\r' << std::flush;
			alive_count = (alive_count + 1)%4;
			step_count++;
			mLog <<"BH: "<< "Jump radius =  " << ampl << "\n";
			mLog.write();
			mRS->next(n,x,ampl,mAux);
			while(!check_bounds(mAux)){
				mRS->next(n,x,ampl,mAux);
				//~ std::cout << "Out of bounds" << std::endl;
				//~ noob++;
			};
			mLog <<"BH: "<< "Trying to jump to: " ;
			for(int j = 0; j < n; j ++) {
				mLog << mAux[j] << " ";
			};
			mLog << "\n";
			mLog.write();
			fAux = mOpt->getObjective()->func(mAux);
			mLog <<"BH: "<< "Constrain function = " << fAux << " ";
			if (fAux< 0){
				mLog << "Valid point. Starting linear search.\n";
				mLog.write();
				if(mOpt->search(mAux, v)) {
					*v = mObj->func(mAux);
					if(*v < vbest) {
						FT d = 0.;
						for(int j = 0; j < n; j ++) {
							FT r = x[j] - mAux[j];
							d += r * r;
						};
						for(int j = 0; j < n; j ++) {
							x[j] = mAux[j];
						};
						mLog <<"BH: "<< "Improved from " << vbest << " to " << *v <<", d = " << sqrt(d) << ", nof = " << i <<"\n";
						std::cout <<"BH: "<< "Improved from " << vbest << " to " << *v <<", d = " << sqrt(d) << ", nof = " << i <<std::endl;
						mLog.write();
						nos ++;
						vbest = *v;
						if (vbest < very_best){
							very_best = vbest;
							for(int j = 0; j < n; j ++) {
								mBest[j] = x[j];
							};
						}
						i = 0;
					} else {
						nof++;
						FT d = 0.;
						for(int j = 0; j < n; j ++) {
							FT r = x[j] - mAux[j];
							d += r * r;
						}
						if (d > epsilon){
							rand_num = ((double)rand())/ ((double)RAND_MAX);
							FT th = mAPF->prob(*v-vbest);
							if (rand_num < th){
								for(int j = 0; j < n; j ++) {
									x[j] = mAux[j];
								}
								mLog <<"BH: " << "Worsened from " << vbest << " to " << *v <<", d = " << sqrt(d) << "\n";
								mLog <<"BH: " << "Random number:  " << rand_num << " ;threshold: " << th << "\n";
								mLog.write();
								vbest = *v;
								//~ i = 0;
							} else {
								mLog <<"BH: " << "Rejected worsening jump from " << vbest << " to " << *v <<", d = " << sqrt(d) << "\n";
								mLog.write();
							}
						}
						
					}
				} else {          
					mLog << "BH: " << ": Failed to improve locally.\n";
					mLog.write();
				}
			} else {
				mLog << "Not a valid point.\n";
				mLog.write();
				nof++;
			}
			T = mTempSched->getParam (step_count, nos, nof, x, vbest, T);
			mAPF->setParam(T);
			ampl = mRangeSched->getParam (step_count, nos, nof, x, vbest, ampl);
		}
		
		mLog <<"BH: " << "Local search initiated " << step_count << " times\n";
		//~ mLog <<"BH: " << "Out of bounds steps taken: " << noob << "\n";
		mLog << "==================\n";
		mLog.write();
		*v = very_best;

		mLog <<"BH: MARK_A: "<< "Best found point: " ;
		for(int j = 0; j < n; j ++) {
			mLog << x[j] << " ";
		};
		mLog << "\n";
		mLog.write();
		
		for(int j = 0; j < n; j ++) {
			x[j] = mBest[j];
		}
		
		mLog <<"BH: MARK_A: "<< "Returning point: " ;
		for(int j = 0; j < n; j ++) {
			mLog << x[j] << " ";
		};
		mLog << "\n";
		mLog.write();
		return (nos != 0);

	}

	/**
	* Set up number of search starts
	* @param n number of starts
	*/
	void setNumberOfStarts(int n){
		mNumberOfStarts = n;
	}


	/**
	* Set up local optimizer
	* @param  local optimizer to set
	*/
	void setLocalOptimizer(LocalOptimizer<FT>* opt){
		mOpt = opt;
		mDim = mOpt->getObjective()->getDim();
		mAux = (FT*) realloc(mAux, mDim * sizeof(FT));
		mBest = (FT*) realloc(mBest, mDim * sizeof(FT));
		l_bound = (FT*) realloc(l_bound, mDim * sizeof(FT));
		u_bound = (FT*) realloc(u_bound, mDim * sizeof(FT));
		for(int j = 0; j < mDim; ++j){
			l_bound[j] = 0.0;
			u_bound[j] = 100.0;
		};
		mObj = mOpt->getObjective();
	}

	/**
	* Set up objective function if should be different from the one used in local optimizer
	* @param  objective function to set
	*/
	void setObjectiveFunction(Objective<FT>* obj){
		mObj = obj;
	}

	/**
	* Set up accept probability function
	* @param apf APF to set
	*/
	void setAcceptProbabilityFunction(AcceptProbabilityFunction<FT>* apf){
		mAPF = apf;
	}
	
	void setTemperatureSchedule(Schedule<FT>* s){
		mTempSched = s;
	}
	
	void setRangeSchedule(Schedule<FT>* s){
		mRangeSched = s;
	}
	
	void setRandomStep(RandomStep<FT>* RS){
		mRS = RS;
	}
	
	void setInitStepLength(FT isl){
		init_step_length = isl;
		mLog << "BH: Setting init_step_length: " << init_step_length << "\n";
		mLog.write();
	}
	
	//~ void setStatParams(FT min_, FT max_, int NumOfLengths){
		//~ min_sl = min_;
		//~ max_sl = max_
		//~ mNumOfLengths = NumOfLengths;
	//~ }	
	
	bool check_bounds(FT* x){
		for(int j = 0; j < mDim; ++j){
			if (x[j] < l_bound[j])
				return false;
			if (x[j] > u_bound[j])
				return false;
		};
		return true;
	};
	

private:
	int mNumberOfStarts;
	FT init_step_length,mDim;
	LocalOptimizer<FT>* mOpt;
	AcceptProbabilityFunction <FT>* mAPF;
	Schedule <FT>* mTempSched;
	Schedule <FT>* mRangeSched;
	RandomStep<FT> * mRS;
	Objective<FT>* mObj;

	BNBLog <BNB_NO_LOGGING> mLog;
	FT *mAux;  
	FT *mBest;
	FT *l_bound;
	FT *u_bound;
};


#endif
