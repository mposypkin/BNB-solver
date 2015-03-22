#ifndef __POWELL_HPP
#define __POWELL_HPP

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "locoptim.hpp"
#include "powelllinesearch.hpp"

#include "util/common/vec.hpp"
#include "util/common/util.hpp"
#include "util/common/utilmacro.hpp"
#include <util/common/bnblog.hpp>
#define LINIEAR_DEP_ZERO 1e-8


//~ template <typename FT> class reverse :  public Objective<FT>{
//~ public:
	//~ reverse(Objective<FT> *obj){
		//~ mObj = obj;
		//~ setDim(mObj->getDim());
	//~ };
	
	
	//~ FT func(const FT * x){
		//~ return -1.0*mObj->func(x);
	//~ };

//~ private:
	//~ Objective<FT> *mObj;
//~ };

template <typename FT> class Powell : public LocalOptimizer<FT>{
	public:
		/**
		* Constructor
		*/
		Powell(){
			mMaxk = -1;
			mDir = NULL;
			mOldDir = NULL;
			yf = NULL;
			yl = NULL;
			new_dir = NULL;
			temp_dir = NULL;
			temp_vect = NULL;
			mEps = 1e-6;
			
			rev = NULL;
			
		}

		/**
		* Setup objective
		* @param obj objective
		*/
		void setObjective(Objective<FT>* obj){
			LocalOptimizer<FT>::setObjective(obj);
			//~ rev = new reverse<FT>(obj);
			gls.setObjective(obj);
			dim = (int)LocalOptimizer<FT>::mObj->getDim();
			mDir = (FT*)realloc(mDir, (dim+1) * dim * sizeof(FT));
			mOldDir = (FT*)realloc(mOldDir, (dim+1) * dim * sizeof(FT));
			
			yf = (FT*) realloc (yf, dim * sizeof(FT));
			yl = (FT*) realloc (yl, dim * sizeof(FT));
			new_dir = (FT*) realloc (new_dir, dim * sizeof(FT));
			temp_dir = (FT*) realloc (new_dir, dim * sizeof(FT));
			temp_vect = (FT*) realloc (temp_vect ,dim*dim * sizeof(FT));
			//ForDocking!!!
			
			
			//~ for(int i = 0; i < dim+1; ++i){
				//~ for(int j = 0; j < dim; ++j){
					//~ std::cout<< mDir[i*dim+j] << " ";
				//~ };
				//~ std::cout << std::endl;
			//~ };
		}
		void setLineSearchStep(FT step){
			gls.setDx (step);
		};


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

			FT norm, normp, w, wold, tau;
			
						
			//~ dim = LocalOptimizer<FT>::mObj->getDim();
			
			for(int j = 0; j < dim-1; ++j){
				mDir[j] = 0.0;
			};
			mDir[dim-1] = 1.0;
			
			for(int i = 1; i <= dim; ++i){
				for(int j = 0; j < dim; ++j){
					if ((i-1) == j){
						mDir[(i)*dim+j] = 1.0;
					} else {
						mDir[(i)*dim+j] = 0.0;
					};
				};
			};

			
			for(;;){
				
				
				if((mMaxk != -1) && (k > mMaxk)){
					VecUtils::vecCopy<FT>(dim, yl, x);
					*v = LocalOptimizer<FT>::mObj->func(x);
					return false;
				}
								
				VecUtils::vecCopy<FT>(dim, x, yf);
				
				VecUtils::vecCopy<FT>(dim, mDir, temp_dir);
				tau = gls.search(yf,temp_dir);
				VecUtils::vecSaxpy<FT>(dim, yf, temp_dir, tau, yf);
				
				w = LocalOptimizer<FT>::mObj->func(yf);	

				mLog << "Powell: "<< "f= "<< w << "; x= ";
				for(int j = 0; j < dim; ++j){
					mLog << x[j] << " ";
				};
				mLog <<"\n";
				mLog.write();
				
				
				
				VecUtils::vecCopy<FT>(dim, yf, yl);
				
				for(int i = 1; i < dim; ++i){
					
					VecUtils::vecCopy<FT>(dim, mDir+dim*i, temp_dir);
					tau = gls.search(yl,temp_dir);
					VecUtils::vecSaxpy<FT>(dim, yl, temp_dir, tau, yl);
				};

				if (VecUtils::vecDist<FT>(dim, x, yl) < mEps){
					VecUtils::vecCopy<FT>(dim, yl, x);
					*v = LocalOptimizer<FT>::mObj->func(x);
					return true;
				};

				VecUtils::vecCopy<FT>(dim, mDir+dim*dim, temp_dir);
				tau = gls.search(yl,temp_dir);
				VecUtils::vecSaxpy<FT>(dim, yl, temp_dir, tau, yl);
				if (VecUtils::vecDist<FT>(dim, yf, yl) < mEps){
					VecUtils::vecCopy<FT>(dim, yl, x);
					*v = LocalOptimizer<FT>::mObj->func(x);
					return true;
				};
				
				
				if (VecUtils::vecDist<FT>(dim, x, yl) < mEps){
					VecUtils::vecCopy<FT>(dim, yl, x);
					*v = LocalOptimizer<FT>::mObj->func(x);
					return true;
				};
				
				VecUtils::vecCopy<FT>(dim, yl, x);
				
				VecUtils::vecSaxpy<FT>(dim, yl, yf, -1.0, new_dir);
				VecUtils::vecCopy<FT>((dim+1)*dim, mDir, mOldDir);
				

				SwitchDirections(new_dir); 
				

				if (isLinearDependant(dim, mDir)){
					VecUtils::vecCopy<FT>((dim+1)*dim, mOldDir, mDir);
				};

				k ++;
				
			}
		}

		~Powell(){
			if(mDir)
				free(mDir);
			if(mOldDir)
				free(mOldDir);
			if(yl)
				free(yl);
			if(yf)
				free(yf);
			if(new_dir)
				free(new_dir);
			
		}
	private:
		FT mEps;
		FT mMaxk;
		FT *mDir;
		FT *mOldDir;
		FT *yl;
		FT *yf;
		FT *new_dir;
		FT *temp_dir;
		FT *temp_vect;
	
		int dim;
		PowellLineSearch<FT> gls;
		Objective<FT>* rev;
		BNBLog <BNB_DO_LOGGING> mLog;	
	
		bool isLinearDependant(int dim, FT* vectors){
			int num = dim;
			for(int j = 0; j < dim*num; ++j){
				temp_vect[j] = vectors[j];
			};
			
			FT max1, cand,coef;
			int max1pos;
			for(int i = 0; i < num; ++i){
				max1pos = 0;
				max1 = (BNBABS(temp_vect[dim*i+0])<1.0) ? BNBABS(temp_vect[dim*i+0]) : 1.0/BNBABS(temp_vect[dim*i+0]);
				for(int j = 1; j < dim; ++j){
					cand = (BNBABS(temp_vect[dim*i+j]<1.0)) ? BNBABS(temp_vect[dim*i+j ]) : 1.0/BNBABS(temp_vect[dim*i+j]);
					if (cand > max1) {
						max1 = cand;
						max1pos = j;
					};
				};
				
				if (max1 < LINIEAR_DEP_ZERO) 
					return true;
				max1 = temp_vect[dim*i + max1pos];
				for(int j = 0; j < dim; ++j){
					temp_vect[dim*i+j] /= max1;
				};
				
				for(int i1 = i+1; i1 < num; ++i1){
					coef = temp_vect[dim*i1+max1pos];
					for(int j = 0; j < dim; ++j){
						temp_vect[dim*i1+j] -= coef * temp_vect[dim*i+j];
					};
				};
			};
			return false;
		};
		
		void SwitchDirections(FT* new_dir){
			for(int i = 1; i < dim; ++i){
				for(int j = 0; j < dim; ++j){
					mDir[i*dim + j] = mDir[(i+1)*dim + j];
				};
			};
			for(int j = 0; j < dim; ++j){
				mDir[0*dim + j] = new_dir[j];
				mDir[(dim)*dim + j] = new_dir[j];
			};
			
		};

};
#endif
