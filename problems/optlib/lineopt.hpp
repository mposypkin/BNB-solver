#ifndef __LINEOPT_HPP
#define __LINEOPT_HPP

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "locoptim.hpp"
#include "linesearch.hpp"

#include "util/common/vec.hpp"
#include "util/common/util.hpp"
#include "util/common/utilmacro.hpp"
#include <util/common/bnblog.hpp>
#define LINIEAR_DEP_ZERO 1e-8



template <typename FT> class LineOptimizer : public LocalOptimizer<FT>{
	public:
		/**
		* Constructor
		*/
		LineOptimizer(){
			mDir = NULL;
			mLs = NULL;
		}

		/**
		* Setup objective
		* @param obj objective
		*/
		
		void setLineSearch(LineSearch<FT>* ls){
			mLs = ls;
		};
		void setObjective(Objective<FT>* obj){
			LocalOptimizer<FT>::setObjective(obj);
			mLs->setObjective(obj);
			dim = (int)LocalOptimizer<FT>::mObj->getDim();
		};

		void setDir(FT* dir){
			mDir = dir;
		};


		/**
		* Perform search
		* @param x start point and result
		* @param v pointer to the resulting value
		* @return true if search converged and false otherwise
		*/
		bool search(FT* x, FT* v){
			a = mLs->search(x,mDir);
			VecUtils::vecSaxpy(dim, x, mDir, a, x);
			*v = LocalOptimizer<FT>::mObj->func(x);
			return true;
		}

		~LineOptimizer(){
			
		}
	private:
	
		int dim;
		FT val,a;
		FT* mDir;
		LineSearch<FT>* mLs;
		BNBLog <BNB_NO_LOGGING> mLog;	
};
#endif
