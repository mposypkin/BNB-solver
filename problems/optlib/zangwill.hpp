#ifndef _ZANGWILL_HPP
#define _ZANGWILL_HPP

#include <stdio.h>
#include <stdlib.h>
#include "locoptim.hpp"
#include "goldls.hpp"
#include "util/common/vec.hpp"
#include "util/common/util.hpp"

template <typename FT> class Zangwill : public LocalOptimizer<FT>{
	public:
		/**
		* Constructor
		*/
		Zangwill(){
			mMaxk = -1;
			mDir = NULL;
			mOldDir = NULL;
			y = NULL;
			z = NULL;
			new_dir = NULL;
		}

		/**
		* Setup objective
		* @param obj objective
		*/
		void setObjective(Objective<FT>* obj){
			LocalOptimizer<FT>::setObjective(obj);
			gls.setObjective(obj);
			dim = (int)LocalOptimizer<FT>::mObj->getDim();
			mDir = (FT*)realloc(mDir, (dim+1) * dim * sizeof(FT));
			mOldDir = (FT*)realloc(mOldDir, (dim+1) * dim * sizeof(FT));

			y = (FT*) realloc (y, dim * sizeof(FT));
			z = (FT*) realloc (z, dim * sizeof(FT));
			new_dir = (FT*) realloc (new_dir,dim * sizeof(FT));
			
			//~ for(int j = 0; j < dim-1; ++j){
				//~ mDir[j] = 0.0;
			//~ };
			
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
			int k = 0;
			int i = 0;

			FT norm, normp, w, wold, lambda,mu;
						
			//~ dim = LocalOptimizer<FT>::mObj->getDim();
			w = LocalOptimizer<FT>::mObj->func(x);
			
			VecUtils::vecCopy<FT>(dim, x, y);
			LocalOptimizer<FT>::mObj->grad(y, mDir+0);
			VecUtils::revert<FT>(dim, mDir+0);
			
			for(;;){
				if((mMaxk != -1) && (k > mMaxk)){
					printf("k: %d (eps = %lf)\n", k, mEps);
					return false;
				}
								
				for(int j = 1; j < dim; ++j){
					lambda = gls.search(y,mDir+(j-1)*dim);
					VecUtils::vecSaxpy<FT>(dim, y, mDir+(j-1)*dim, lambda, y);
					
					LocalOptimizer<FT>::mObj->grad(y, new_dir);
					VecUtils::revert<FT>(dim, new_dir);
					
					mu = gls.search(y,new_dir);
					VecUtils::vecSaxpy<FT>(dim, y, new_dir, mu, z);
					
					for(int i = 1; i <= j; ++i){
						LocalOptimizer<FT>::mObj->grad(z, new_dir);
						if (VecUtils::vecNormTwo<FT>(dim, new_dir)){
							VecUtils::vecCopy<FT>(dim, z, x);
							*v = LocalOptimizer<FT>::mObj->func(x);
							return true;
						} else {
							mu = gls.search(z,mDir+(i-1)*dim);
							VecUtils::vecSaxpy<FT>(dim, z, mDir+(i-1)*dim, mu, z);
						}
					};
					
					VecUtils::vecSaxpy<FT>(dim, z, y, -1.0, mDir+(j+1-1)*dim);
					
				};
				
				lambda = gls.search(y,mDir+(dim-1)*dim);
				VecUtils::vecSaxpy<FT>(dim, y, mDir+(dim-1)*dim, lambda, y);

				VecUtils::vecCopy<FT>(dim, y, x);
				LocalOptimizer<FT>::mObj->grad(y, mDir+0);
				VecUtils::revert<FT>(dim, mDir+0);
				
				k ++;
			}
		}

		~Zangwill(){
			if(mDir)
				free(mDir);
			if(mOldDir)
				free(mOldDir);
			if(y)
				free(y);
			if(z)
				free(z);
			if(new_dir)
				free(new_dir);
			
		}
	private:
		FT mEps;
		FT mMaxk;
		FT *mDir;
		FT *mOldDir;
	
		FT *y;
		FT *z;
		FT *new_dir;
	
		int dim;
		GoldenLineSearch<FT> gls;

};
#endif
