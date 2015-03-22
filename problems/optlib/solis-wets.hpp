#ifndef __SOLIS_WETS_HPP
#define __SOLIS_WETS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "locoptim.hpp"
#include "util/common/vec.hpp"
#include "util/common/util.hpp"
#include "util/common/random.hpp"
#include "util/common/randomstep.hpp"

template <typename FT> class SolisWetsBasic : public LocalOptimizer<FT>{
	public:
		/**
		* Constructor
		*/
		SolisWetsBasic(){
			mMaxk = -1;
			mns = 5;
			mnf = 3;
			ex = 2.0;
			ct = 0.5;
			r_start = 1.0;
			r_end = 0.001;
			new_x = NULL;
			b = NULL;
			new_b = NULL;
			e = NULL;
			bounds = NULL;
		}

		/**
		* Setup objective
		* @param obj objective
		*/
		void setObjective(Objective<FT>* obj){
			LocalOptimizer<FT>::setObjective(obj);
			dim = (int)LocalOptimizer<FT>::mObj->getDim();
			bounds = (FT*) realloc(bounds, SolisWetsBasic<FT>::rs->GetBoundsSize(SolisWetsBasic<FT>::dim));

			new_x = (FT*) realloc (new_x,dim * sizeof(FT)); 
			b = (FT*) realloc (b,dim * sizeof(FT)); //bias factor;
			new_b = (FT*) realloc (new_b,dim * sizeof(FT)); 
			e = (FT*) realloc (e,dim * sizeof(FT)); //end point of random  steps
			
			
		}


		/**
		* Install epsilon
		* @param eps 
		*/
		void setEps(FT eps){
			mEps = eps;
		}

		/**
		* Install maximal k counter
		* @param k k counter
		*/
		void setMaxk(int k){
			mMaxk = k;
		}

		/**
		* Perform search
		* @param x start point and result
		* @param v pointer to the resulting value
		* @return true if search converged and false otherwise
		*/
		bool search(FT* x, FT* v){

			int k = 0; //number of iterations
			int ns = 0; // number of consequtive success
			int nf = 0; // -//- failures
			
			FT r = r_start; // current "radius" of random search
			
			for(int j = 0; j < dim; ++j){
				b[j] =0;
			};
			FT w_old = LocalOptimizer<FT>::mObj->func(x);
			FT w;
			
			for(;;){
				if((mMaxk != -1) && (k > mMaxk)){
					printf("k: %d (eps = %lf)\n", k, mEps);
					return false;
				}
				
				if (ns >= mns){
					r = r * ex;
				};
				if (nf >= mnf){
					r = r * ct;
				};
				
				if (r < r_end){
					//~ VecUtils::vecCopy<FT>(dim, yl, x);
					*v = LocalOptimizer<FT>::mObj->func(x);
					return true;
				} else {
					//~ printf("Mark1\n");
					rs->GetBounds(dim,r,bounds);
					//~ printf("Mark2\n");
					VecUtils::vecSaxpy<FT>(dim, x,b, 1,new_x);
					//~ printf("Mark3\n");
					rs->next(dim,new_x,bounds,e);
					//~ printf("Mark4\n");
					w = LocalOptimizer<FT>::mObj->func(e);
					if (w < w_old) {
						ns++;
						nf = 0;
						//~ printf("Mark5\n");
						VecUtils::vecSaxpy<FT>(dim, e,x, -1,new_b);
						//~ printf("Mark6\n");
						VecUtils::vecMult<FT>(dim, new_b,0.4,new_b);
						//~ printf("Mark7\n");
						VecUtils::vecSaxpy<FT>(dim, new_b,b, 0.2,b);
						//~ printf("Mark8\n");
						VecUtils::vecCopy<FT>(dim, e, x);
						//~ printf("Mark9\n");
						w_old = w;
					} else {
						VecUtils::vecMult(dim,x,2.0,new_x);
						VecUtils::vecSaxpy<FT>(dim, new_x,e, -1,new_x);
						w = LocalOptimizer<FT>::mObj->func(new_x);
						if (w < w_old){
							ns++;
							nf = 0;
							VecUtils::vecSaxpy<FT>(dim, e,x, -1,new_b);
							VecUtils::vecSaxpy<FT>(dim, b,new_b, -0.4,b);
							VecUtils::vecCopy<FT>(dim, new_x, x);
							w_old = w;
						} else {
							ns = 0;
							nf ++;
							VecUtils::vecMult<FT>(dim, b,0.5,b);
						}
						
					}
					
				};
				k ++;
			}
		}

		~SolisWetsBasic(){
			if(bounds)
				free(bounds);
			if(new_x)
				free(new_x);
			if(b)
				free(b);
			if(new_b)
				free(new_b);
			if(e)
				free(e);
			delete rs;
			
		}
	
		FT mEps;
		FT mMaxk;
		int dim;
		int mns,mnf; // Max number of success/fail
		double ex, ct; // Expand/contract factors
		double r_start,r_end; //End "radius" (desired accuracy)
		RandomStep<FT> *rs;
		FT* bounds; //appropriate bounds for random points generation
		 // set bounds according to given radius
		
	private:
		FT *new_x;
		FT *b;
		FT *new_b;
		FT *e;
};

template <typename FT> class SolisWets1 : public SolisWetsBasic<FT>{
	public:
		SolisWets1(){
			SolisWetsBasic<FT>::rs =  new MultiNormal<FT>;
		};
};

template <typename FT> class SolisWets2 : public SolisWetsBasic<FT>{
	public:
		SolisWets2(){
			SolisWetsBasic<FT>::rs = new UniformCubic<FT>;
		};
};
#endif
