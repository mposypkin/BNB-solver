#ifndef __NULL_OPT_HPP
#define __NULL_OPT_HPP

#include <stdio.h>
#include <stdlib.h>
#include "util/common/vec.hpp"
#include "util/common/util.hpp"

template <typename FT> class NullOpt : public LocalOptimizer<FT>{
	public:
		/**
		* Setup objective
		* @param obj objective
		*/
		void setObjective(Objective<FT>* obj){
			LocalOptimizer<FT>::setObjective(obj);
		}

		/**
		* Perform search
		* @param x start point and result
		* @param v pointer to the resulting value
		* @return always true 
		*/
		bool search(FT* x, FT* v){
			*v = LocalOptimizer<FT>::mObj->func(x);
			return true;
		}

	};
#endif
